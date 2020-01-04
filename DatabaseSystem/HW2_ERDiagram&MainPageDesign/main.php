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
	
	if($_POST)
	{
	    $sql=
		"SELECT users.name 
		FROM users 
	    WHERE uid='".mysqli_real_escape_string($con, $_POST['user'])."' 
	    AND password='".mysqli_real_escape_string($con, md5($_POST['pw']))."' ";
	}
	else
	{
		$sql=
		"SELECT users.name 
		FROM users 
	    WHERE uid='".mysqli_real_escape_string($con, $_SESSION['login_user'])."' 
	    AND password='".mysqli_real_escape_string($con, md5($_SESSION['login_pw']))."' ";
	}
	
	$result=mysqli_query($con, $sql);
	
	$valid_user=0;
	
	while($row=mysqli_fetch_array($result))
	{
	    if(!isset($_SESSION['login_user']) || !isset($_SESSION['login_pw']))
		{
			$_SESSION['login_user']=$_POST['user'];
			$_SESSION['login_pw']=$_POST['pw'];
		}
		
		$valid_user=1;
		
		echo "<h2><font style='background-color:orange'>Welcome <i>".$row['name']."</i> !</font></h2><br/>";
		echo ("<a href=\"http://localhost:7776/userinfo.php?n=$row[name]\">User Info</a>");
		echo " ";
		echo ("<a href=\"http://localhost:7776/logout.php\">Log out</a>");
		echo "<br/><br/>";
		
		echo "<p align=right>Friends</p>";
		
		$sql=
		"SELECT users.name 
		FROM users, friends
		WHERE friends.uid='$_SESSION[login_user]'
		AND friends.friend_id=users.uid";
		
		$result=mysqli_query($con, $sql);
		while($row=mysqli_fetch_array($result))
	    {
		    echo ("<p align=right><a href=\"http://localhost:7776/userinfo.php?n=$row[name]\">$row[name]</a></p>");
		}
		
		$sql=
		"CREATE VIEW orderArticles
		AS SELECT articles.uid,articles.content,articles.time 
		FROM articles 
        ORDER BY articles.time DESC";
		
		$sql1=
		"SELECT DISTINCT users.name AS user,orderArticles.content AS text,orderArticles.time AS time
		FROM users,friends,orderArticles
        WHERE (friends.uid='$_SESSION[login_user]'		
        AND friends.friend_id=users.uid		
		AND orderArticles.uid=friends.friend_id)
		OR 
		(users.uid='$_SESSION[login_user]' 
		AND orderArticles.uid=users.uid)";

		$article=mysqli_query($con, $sql1);	
		
		while($row=mysqli_fetch_array($article))
	   {
	        echo $row['user']; echo " said:<br/>";
		    echo $row['text']; echo "<br/>";
		    echo $row['time']; echo "<br/>";
		    echo "<br/><br/>";
	   }
		
	}
	
	if(!$valid_user)
	{
		echo "Wrong Password<br/>";
		echo ("<a href=\"http://localhost:7776/index.php\">Back</a>");
	}

	mysqli_close($con);
?>	

</body>

</html>