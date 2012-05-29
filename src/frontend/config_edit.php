<?php

$config_file = new conf_edit($input->config);

  if (isset($_POST['submit_config'])) {
      $config_file->writef($_POST);
  }
  if (isset($_POST['restart_katalyzer'])) {
      system("sudo /etc/init.d/katalyzer restart");
  }
  
$config_file->readf();  
?> 

<div id="config_tabs">
<form class="config" action="<?php echo $_SERVER['PHP_SELF']."?mod=edit" ?>" method="post">
  <?php
  foreach($config_file->commets[0] as $commets) {
      echo $commets."<br />";
  }
  
  echo "<ul>Interface:<li><select name=\"INTERFACE\" class=\"long\" />";
  echo "<option  value=\"".$config_file->interface."\">".$config_file->interface."</ option>";
  foreach($config_file->list_interface as $interface) {
    if ($interface != $config_file->interface) {
      echo "<option  value=\"".$interface."\">".$interface."</ option>";
    }
  }
  echo "</select></li></ul>";
  
  foreach($config_file->commets[1] as $commets) {
      echo $commets."<br />";
  }
  
  ?>
  <br />
  <ul>Database Host:<li><input name="DB_HOST" value='<?php echo $config_file->host ?>' class="long" /></li></ul><br />
  <ul>Database name:<li><input name="DB_NAME" value='<?php echo $config_file->database ?>' class="long" /></li></ul><br />
  <ul>Database user:<li><input name="DB_USER" value='<?php echo $config_file->user ?>' class="long" /></li></ul><br />
  <ul>Database password:<li><input type="password" name="DB_PASS" value='<?php echo $config_file->pass ?>' class="long" /></li></ul><br />
  
  <?php
  foreach($config_file->commets[2] as $commets) {
      echo $commets."<br />";
  }
  echo "<table>";
  echo "<tr><td>Protocol:<td></tr>";
  foreach($config_file->protocols as $key => $value) {
    if ($value == 1) {
      echo "<tr><td><input type=\"checkbox\" name=\"protocols[]\" value=\"".strtoupper($key)."\" checked />&nbsp&nbsp&nbsp&nbsp".$key."</td></tr>";  
    }
    else {
      echo "<tr><td><input type=\"checkbox\" name=\"protocols[]\" value=\"".strtoupper($key)."\" />&nbsp&nbsp&nbsp&nbsp".$key."</td></tr>";
    }
  }
  echo "</table>";
  //echo "</li></ul><br />";
  
  $tcp_p="";
  foreach($config_file->tcp as $value) {
      $tcp_p = $tcp_p.$value.";";
  }
  
  $udp_p ="";
  foreach($config_file->udp as $value) {
      $udp_p = $udp_p.$value.";";
  }
  
  foreach($config_file->commets[3] as $commets) {
      echo $commets."<br />";
  }
  ?>
  <table>
  <tr><td>TCP ports:</td></tr><tr><td><input name="tcp_port" value="<?php echo $tcp_p; ?>" /></td></tr>
  
  <tr><td>UDP ports:</td></tr><tr><td><input name="udp_port" value="<?php echo $udp_p; ?>" /></td></tr>
  </table><br />
  
  <input type="submit" name="submit_config" value="Send" />
  <input type="submit" name="restart_katalyzer" value="Restart" />
  
  </form>
  
</div>
