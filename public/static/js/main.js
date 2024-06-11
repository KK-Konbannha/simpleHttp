const makeRequest = () => {
  const xhr = new XMLHttpRequest();

  const result = document.getElementById("result");
  result.innerHTML = "Loading...";

  const port = window.location.port;
  xhr.open("GET", `http://localhost:${port}/api/date`, true);
  xhr.onreadystatechange = () => {
    if (xhr.readyState === 4 && xhr.status === 200) {
      // result.innerHTML = xhr.responseText;
      const data = JSON.parse(xhr.responseText);
      result.innerHTML = data.date;
    }
  };
  xhr.send();

  xhr.onerror = () => {
    console.error("An error occurred.");
  };

  xhr.ontimeout = () => {
    console.error("The request timed out.");
  };
};

const getCount = () => {
  const count = document.getElementById("countValue");
  const xhr = new XMLHttpRequest();

  const port = window.location.port;
  xhr.open("GET", `http://localhost:${port}/api/count`, true);
  xhr.onreadystatechange = () => {
    if (xhr.readyState === 4 && xhr.status === 200) {
      const data = JSON.parse(xhr.responseText);
      count.innerHTML = data.count;
    }
  };
  xhr.send();

  xhr.onerror = () => {
    console.error("An error occurred.");
  };

  xhr.ontimeout = () => {
    console.error("The request timed out.");
  };
};

document.getElementById("ajaxButton").addEventListener("click", makeRequest);

document.addEventListener("DOMContentLoaded", getCount);
