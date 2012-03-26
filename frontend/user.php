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

?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
	"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml"><head>
<script type="text/javascript" src="ajax/jquery-1.3.2.min.js"></script>
<script type="text/javascript" src="ajax/jquery-ui-1.7.2.custom.min.js"></script>
<script type="text/javascript" src="ajax/global.js"></script>

<link rel="stylesheet" type="text/css" href="./themes/style.css" media="screen" />


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

<div id="back">
<a href="../KaTalyzer">Back</a>
</div>

<center>
<?php

$input = new input('configs');
$config = new config($input->config);
  $type[0] = "Admin";
  $type[1] = "User";

$user = new user($config->host, $config->user, $config->pass, "KATALYZER_USER");

if (isset($_POST['delete']) && isset($_POST['user_id'])) {
  $user->del_user($_POST['user_id']);
  unset($_POST);
}

if (isset($_POST['edit_submit_password']) && isset($_POST['user_id'])) {
  $user->edit_user_password($_POST['user_id'], @loggin::login_password_encryption($_POST['passwd']));
  unset($_POST);
}
if (isset($_POST['edit_submit_name']) && isset($_POST['user_id'])) {
  $user->edit_user($_POST['user_id'], $_POST['name'], $_POST['group']);
}

if (isset($_POST['add_user_submit'])) {
       if($user->add_user($_POST['name'], loggin::login_password_encryption($_POST['password']), $_POST['group']))
	 echo "Success</ br>";
	else
	  echo "Error";
}

if (isset($_POST['edit']) && isset($_POST['user_id'])){
  $us=$user->user($_POST['user_id']);
  echo "<div>\n";
  echo "Edit user";
  echo "<form action=".$_SERVER['PHP_SELF']." method='post' name=FormName>";
  echo "<input type='hidden' name='user_id' value='".$us['id']."'>\n";
  echo "Name: <input type='text' name='name' value='".$us['name']."'></br>\n";
  echo "Group: <select name='group'>";
  echo "<option value='".$us['groups']."'>".$type[$us['groups']]."</option>";
  for($i=0;$i<2;$i++) {
    if ($i!=$us['groups']) {
       echo "<option value='".$i."'>".$type[$i]."</option>";
    }
  }
  echo "</select></br>";
  echo "<input type='submit' name='edit_submit_name' value='Send'>";
  echo "</form>";
  echo "</div>\n";
 
  echo "<div>\n";
  echo "Change password:</br>";
  echo "<form action=".$_SERVER['PHP_SELF']." method='post' name=FormPassword onSubmit=\"return validatePwd()\"> </br>\n";
  echo "<input type='hidden' name='user_id' value='".$us['id']."'>\n";
  echo "Password: <input type='password' name='password'></br>\n";
  echo "Validate password: <input type='password' name='password2'></br>";
  echo "<input type='submit' name='edit_submit_password' value='Send'>";
  echo "</form>";
  echo "</div>\n";
}
elseif(isset($_GET['action'])) {
   echo "<div>\n";
  echo "<form action=".$_SERVER['PHP_SELF']." method='post' name=Form onSubmit=\"return validatePwd()\"> </br>\n";
  echo "Name: <input type='text' name='name' value=''></br>\n";
  echo "Group: <select name='group'>";
  echo "<option value=0>Admin</option>";
  echo "<option value=1>User</option>";
  echo "</select></br>";
  echo "Password: <input type='password' name='password'></br>\n";
  echo "Validate password: <input type='password' name='password2'></br>";
  echo "<input type='submit' name='add_user_submit' value='Send'>";
  echo "</form>";
  echo "</div>\n"; 
}
else {
  
  echo "<div>\n";
  
  echo "<table>\n";
  echo "<tr><td>Name</td><td>Group</td><td>Edit</td><td>Delete</td></tr>\n";
  foreach($user->users() as $key => $value) {
    echo "<tr>";
    echo "<td>".$value['name']."</td>";
    echo "<td>".$type[$value['groups']]."</td>";
    echo "<td><form action=".$_SERVER['PHP_SELF']." method='post'><input type='hidden' name='user_id' value='".$key."'><input type='submit' name='edit' value='Edit'></td><td><input type='submit' name='delete' value='Delete'></form></td>";
    echo "</tr>\n";
  }
  echo "<tr><td></td><td></td><td><a href='?action=add'> Add user</a></td></tr>";
  
  echo "</table>\n";
  
  echo "</div>\n";
}

?>
</center>
</body>
</html>
<?php
}
?>