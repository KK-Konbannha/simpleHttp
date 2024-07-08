use std::env;
use std::fs::OpenOptions;
use std::io::{self, Write};
use std::process::Command;
use std::str::FromStr;

fn main() -> io::Result<()> {
    // コマンドライン引数を取得
    let args: Vec<String> = env::args().collect();
    if args.len() != 8 {
        eprintln!("Usage: {} <output_file> <max_i> <max_j> <iterations> <ip> <port> <is_keep_alive>", args[0]);
        std::process::exit(1);
    }

    let output_file = &args[1];
    let max_i: u32 = args[2].parse().expect("max_i should be a number");
    let max_j: u32 = args[3].parse().expect("max_j should be a number");
    let iterations: u32 = args[4].parse().expect("iterations should be a number");
    let ip = &args[5];
    let port: u32 = args[6].parse().expect("port should be a number");
    let is_keep_alive: bool = args[7].parse().expect("is_keep_alive should be a boolean");

    // CSVファイルを開き、追記モードで準備
    let mut file = OpenOptions::new()
        .append(true)
        .create(true)
        .open(output_file)
        .expect("Cannot open file");

    // ヘッダを書き込み
    writeln!(file, "Total Connections,Concurrent Connections,Time taken for tests(s),Failed requests,Time per request(ms),Time per request across all concurrent requests(ms)")?;

    for i in (0..=max_i).map(|x| 2_usize.pow(x)) {
        for j in (0..=max_j).map(|x| 2_usize.pow(x)).take_while(|&j| j <= i) {
            let mut total_time_taken = 0.0;
            let mut total_failed_requests = 0;
            let mut total_time_per_request = 0.0;
            let mut total_time_per_request_across_request = 0.0;

            for _ in 0..iterations {
                let mut command = Command::new("ab");
                command.arg("-n")
                       .arg(i.to_string())
                       .arg("-c")
                       .arg(j.to_string())
                       .arg("-s")
                       .arg("3");

                if is_keep_alive {
                    command.arg("-k");
                }

                command.arg(format!("http://{}:{}/", ip, port));

                let output = command.output()
                    .expect("Failed to execute ab command");

                let output_str = String::from_utf8_lossy(&output.stdout);

                // 必要な情報を抽出
                let time_taken = output_str.lines()
                    .find(|line| line.starts_with("Time taken for tests"))
                    .and_then(|line| line.split_whitespace().nth(4))
                    .and_then(|s| f64::from_str(s).ok())
                    .unwrap_or(0.0);

                let failed_requests = output_str.lines()
                    .find(|line| line.starts_with("Failed requests"))
                    .and_then(|line| line.split_whitespace().nth(2))
                    .and_then(|s| usize::from_str(s).ok())
                    .unwrap_or(0);

                let time_per_request = output_str.lines()
                    .find(|line| line.starts_with("Time per request") && line.contains("(mean)"))
                    .and_then(|line| line.split_whitespace().nth(3))
                    .and_then(|s| f64::from_str(s).ok())
                    .unwrap_or(0.0);

                let time_per_request_across_request = output_str.lines()
                    .find(|line| line.starts_with("Time per request") && line.contains("(mean, across all concurrent requests)"))
                    .and_then(|line| line.split_whitespace().nth(3))
                    .and_then(|s| f64::from_str(s).ok())
                    .unwrap_or(0.0);

                total_time_taken += time_taken;
                total_failed_requests += failed_requests;
                total_time_per_request += time_per_request;
                total_time_per_request_across_request += time_per_request_across_request;
            }

            let avg_time_taken = total_time_taken / iterations as f64;
            let avg_failed_requests = total_failed_requests as f64 / iterations as f64;
            let avg_time_per_request = total_time_per_request / iterations as f64;
            let avg_time_per_request_across_request = total_time_per_request_across_request / iterations as f64;

            // CSVファイルに書き込み
            writeln!(
                file,
                "{},{},{:.3},{},{:.3},{:.3}",
                i, j, avg_time_taken, avg_failed_requests, avg_time_per_request, avg_time_per_request_across_request
            )?;
        }
    }

    Ok(())
}
