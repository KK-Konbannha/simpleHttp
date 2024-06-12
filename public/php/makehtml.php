<?php

$files = glob("static/img/*.jpg");

foreach($files as $file){
  echo "<img src=\"$file\" width=\"100\">\n";
}

?>
