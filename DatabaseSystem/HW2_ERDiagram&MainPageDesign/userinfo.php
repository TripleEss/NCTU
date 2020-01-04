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
	$database="db_pj2";
	$con=mysqli_connect($host, $username, $password, $database);
	
	if(!$_SESSION)
	{	
		echo "you havent't logged in yet<br/>";
		echo ("<a href=\"http://localhost:7776/index.php\">Back</a>");
	
	}
	else
	{
		$sql="SELECT users.name AS name,users.email AS email,users.birthday AS birth
		FROM users 
		WHERE users.name='$_GET[n]'";
		
		$user=mysqli_query($con, $sql);
		
		while($row=mysqli_fetch_array($user))
		{
		    echo "<br/><br/>";
			echo "<p align=center><b>".$row['name']."</b></p><br/>";
			echo "<p align=center>Email: ".$row['email']."</p>";
			echo "<p align=center>Birthday: ".$row['birth']."</p><br/>";
		}
		
		
		echo ("<p align=center><a href=\"http://localhost:7776/main.php\">Back to main page</a></p>");
		echo ("<p align=center><a href=\"http://localhost:7776/logout.php\">Log out</a></p>");		
		
		mysqli_close($con);
	}
?>
	
</body>

</html>