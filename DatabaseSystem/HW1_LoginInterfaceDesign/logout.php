<!DOCTYPE html>
<?php
	session_start();
	if(isset($_SESSION['username']))
		unset($_SESSION['username']);
	if(isset($_SESSION['password']))
		unset($_SESSION['password']);
 ?> 
<html>

<head>
</head>

<body>

	Goodbye!
	<br/><br/>
	<a href="http://localhost:7776/index.php">Back</a>
	<br/>
	
</body>

</html>