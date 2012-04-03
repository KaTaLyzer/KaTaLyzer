<?php

session_start();

if (isset($_SESSION['logged']) AND $_SESSION['logged'] == true AND isset($_SESSION['group']) AND $_SESSION['group'] == 0) {

define("INTERVAL", 86400);
define("PAGE",25);
require_once('inc/class_input.php');
require_once('inc/class_config.php');
require_once('inc/class_port_names.php');
require_once('inc/class_db.php');
require_once('inc/class_svg.php');
require_once 'inc/class_loggin.php';

require_once './inc/class_user.php';
require_once 'inc/class_edit_conf.php';

$input = new input('configs');
$config = new config($input->config);


?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
	"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml"><head>
<script type="text/javascript" src="ajax/jquery-1.3.2.min.js"></script>
<script type="text/javascript" src="ajax/jquery-ui-1.7.2.custom.min.js"></script>
<script type="text/javascript" src="ajax/global.js"></script>

<link rel="stylesheet" type="text/css" href="<?php echo $input->theme; ?>/style.css" media="screen" />

<SCRIPT LANGUAGE="JavaScript">
<!-- Original:  Russ Swift (rswift220@yahoo.com) -->

<!-- This script and many more are available free online at -->
<!-- The JavaScript Source!! http://www.javascriptsource.com -->

<!-- Begin
function validatePwd() {
  var invalid = " "; // Invalid character is a space
  var minLength = 4; // Minimum length
  var pw1 = document.FormPassword.password.value;
  var pw2 = document.FormPassword.password2.value;
  // check for a value in both fields.
  if (pw1 == '' || pw2 == '') {
    alert('Please enter your password twice.');
    return false;
  }
  // check for minimum length
  if (document.FormPassword.password.value.length < minLength) {
    alert('Your password must be at least ' + minLength + ' characters long. Try again.');
    return false;
  }
  // check for spaces
  if (document.FormPassword.password.value.indexOf(invalid) > -1) {
    alert("Sorry, spaces are not allowed.");
    return false;
  }
  else {
    if (pw1 != pw2) {
      alert ("You did not enter the same new password twice. Please re-enter your password.");
      return false;
    }
    else {
      return true;
    }
  }
}
//  End -->
</script>

</head>
<body>
<div class="container">
<div id="top_menu">

	<div id="nadpis">
		<h1><a href=".">KaTaLyzer</a></h1>
	</div>
	<div id="navlist"><div id="navlist_left">
		<a href="./index.php">Home</a>
<?php
  
if ($_SESSION['group'] == 0) {

?>
		<a class="small" href="./managment.php?mod=user">Users</a>
		<a class="small" href="./managment.php?mod=edit">Edit config</a>
<?php
}
?>
		<a class="small" href="<?php echo $_SERVER['PHP_SELF']."?act=out" ?>">Logout</a>
	</div></div>
</div>

<div id="top_container">

<?php
  if (isset($_GET['mod']) && ($_GET['mod'] == 'user')) {
      require_once './user.php';
  }elseif
   (isset($_GET['mod']) && ($_GET['mod'] == 'edit')) {
     require_once './config_edit.php';
  }
  else {
      require_once './user.php';
  }
  
?>

</div>
</div>


</body>
</html>
<?php
}
?>

