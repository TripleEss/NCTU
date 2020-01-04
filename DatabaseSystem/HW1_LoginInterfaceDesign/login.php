<!DOCTYPE html>
<?php 
	session_start(); 
?>
<html>

<head>
</head>

<body>
<?php

	$host="localhost";
	$username="dbuser";
	$password="dbuser";
	$database="db_pj1";
	$con=mysqli_connect($host, $username, $password, $database);
	
	$sql="SELECT username FROM users 
	WHERE username='".mysqli_real_escape_string($con, $_POST['user'])."' 
	AND password='".mysqli_real_escape_string($con, md5($_POST['pw']))."' ";
	
	$result=mysqli_query($con, $sql);
	
	$valid_user=0;
	while($row=mysqli_fetch_array($result))
	{
		echo "Welcome ".$row['username']; echo "!<br/>";
		echo ("<a href=\"http://localhost:7776/userinfo.php\">User Info</a>");
		echo "<br/><br/>";
		echo ("<a href=\"http://localhost:7776/logout.php\">Log out</a>");
		echo "<br/>";
		
		$_SESSION['username']=$_POST['user'];
		$_SESSION['password']=$_POST['pw'];
		$valid_user=1;
	}
	
	if(!$valid_user)
	{
		echo "Wrong Password<br/>";
	}

	mysqli_close($con);
?>	

</body>

</html>