<?php

require_once './inc/class_loggin.php';

class user extends database{
    
  public function __construct($servername, $username, $password, $database){
    $this->set_server($servername);
    $this->set_username($username);
    $this->set_password($password);
    $this->set_database($database);
    $this->connect();
  }
  
  public function  users(){
    $sql = "SELECT id, name, groups FROM USER";
    $this->query($sql);
   while ($row = $this->fetch()) {
       $user[$row['id']]['name']=$row['name'];
       $user[$row['id']]['groups']=$row['groups'];
   }
    return $user;
  }
  
  public function user($id){
    $sql = "SELECT id, name, groups FROM USER WHERE id='".$id."'";
    $this->query($sql);
    return $this->fetch();
  }
  
  public function del_user($id){
    $sql = "DELETE FROM USER WHERE id='".$id."'";
    $return = $this->query($sql);
    if ($return) {
        return true;
    }
    else {
        return false;
    }
  }
  
  public function edit_user($id, $name, $group){
    $sql = "UPDATE USER SET name='".$name."', groups='".$group."' WHERE id='".$id."'";
    $return = $this->query($sql);
    if ($return) {
        return true;
    }
    else {
        return false;
    }
  }
  
  public function edit_user_password($id, $passwd){
    $sql = "UPDATE USER SET password='".$passwd."' WHERE id='".$id."'";
    $return = $this->query($sql);
    if ($return) {
        return true;
    }
    else {
        return false;
    }
  }
  
  public function add_user($name, $passwd,$group){
    $sql = "INSERT INTO USER (name, password, groups) VALUES('".$name."', '".$passwd."', '".$group."')";
    echo $sql;
    $return = $this->query($sql);
    if ($return) {
        return true;
    }
    else {
        return false;
    }
  }
  
}


?>