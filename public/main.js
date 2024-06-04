document.getElementById("ajaxButton").addEventListener("click", makeRequest);

const makeRequest = () => {
  console.log("Button clicked");
  // const xhr = new XMLHttpRequest();
  // xhr.onreadystatechange = () => {
  //   if (xhr.readyState === 4) {
  //     if (xhr.status === 200) {
  //       console.log(xhr.responseText);
  //     } else {
  //       console.log("Error: " + xhr.status);
  //     }
  //   }
  // };
  // xhr.open("GET", "data.txt", true);
  // xhr.send();
};
