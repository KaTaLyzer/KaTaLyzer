<?php

  $type[0] = "Admin";
  $type[1] = "User";

$user = new user($config->host, $config->user, $config->pass, "KATALYZER_USER");

if (isset($_POST['delete']) && isset($_POST['user_id'])) {
  $user->del_user($_POST['user_id']);
  unset($_POST);
}

if (isset($_POST['edit_submit_password']) && isset($_POST['user_id'])) {
  $user->edit_user_password($_POST['user_id'], @loggin::login_password_encryption($_POST['password']));
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
  echo "<form action=".$_SERVER['PHP_SELF']."?mod=user method='post' name=FormName>";
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
  echo "<form action=".$_SERVER['PHP_SELF']."?mod=user method='post' name=FormPassword onSubmit=\"return validatePwd()\"> </br>\n";
  echo "<input type='hidden' name='user_id' value='".$us['id']."'>\n";
  echo "Password: <input type='password' name='password'></br>\n";
  echo "Validate password: <input type='password' name='password2'></br>";
  echo "<input type='submit' name='edit_submit_password' value='Send'>";
  echo "</form>";
  echo "</div>\n";
}
elseif(isset($_GET['action'])) {
   echo "<div>\n";
  echo "<form action=".$_SERVER['PHP_SELF']."?mod=user method='post' name=FormPassword onSubmit=\"return validatePwd()\"> </br>\n";
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
    echo "<td><form action=".$_SERVER['PHP_SELF']."?mod=user method='post'><input type='hidden' name='user_id' value='".$key."'><input type='submit' name='edit' value='Edit'></td><td><input type='submit' name='delete' value='Delete'></form></td>";
    echo "</tr>\n";
  }
  echo "<tr><td></td><td></td><td><a href='?mod=user&action=add'> Add user</a></td></tr>";
  
  echo "</table>\n";
  
  echo "</div>\n";
}

?>