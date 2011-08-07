<?php
session_start();
Header("Pragma: no-cache");
Header("Cache-Control: no-cache");
Header("Expires: ".GMDate("D, d M Y H:i:s")." GMT");

header($_SESSION['image_header']);
echo $_SESSION['image'];

?>
