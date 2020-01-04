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
	
	if($_SESSION)
	{	
		$sql1="SELECT username FROM users 
		WHERE username='".mysqli_real_escape_string($con, $_SESSION['username'])."'
		AND password='".mysqli_real_escape_string($con, md5($_SESSION['password']))."' ";
		
		$user=mysqli_query($con, $sql1);
		
		while($row=mysqli_fetch_array($user))
		{
			echo "You are ".$row['username']; echo "<br/>";
		}
		
		$sql2="SELECT email FROM users 
		WHERE username='".mysqli_real_escape_string($con, $_SESSION['username'])."'
		AND password='".mysqli_real_escape_string($con, md5($_SESSION['password']))."' ";
		
		$email=mysqli_query($con, $sql2);
		
		while($row=mysqli_fetch_array($email))
		{
			echo "Email: ".$row['email']; echo "<br/>";
		}
		echo ("<a href=\"http://localhost:7776/logout.php\">Log out</a>");
		
		mysqli_close($con);
	}
	else
	{
		echo "you havent't logged in yet";
	}
?>
	
</body>

</html>