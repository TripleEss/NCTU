<!DOCTYPE html>
<?php
	session_start();
	if(isset($_SESSION['login_user']) || isset($_SESSION['login_pw']))
		session_destroy();
 ?> 
<html>

<head>
</head>

<body>
    <br/><br/>
	<h1><p align=center><font color="#ff8000">Goodbye!</font></p></h1>
	<br/><br/>
	<p align=center><a href="http://localhost:7776/index.php">Back</a></p>
	<br/>
	
</body>

</html>