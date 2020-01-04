<!DOCTYPE html>
<?php 
	session_start(); 
?>
<html>

<head>
	<title>Facenote</title>
</head>

<body>
<?php
    if(!$_POST)
    { 
        echo
	"
            <p align=center><font color=#ff8000>----------------------------------------------</font></p>
        <h1><p align=center><font color=#ff0000>                 <b>FACENOTE</b>              </font></p></h1>
            <p align=center><font color=#ff8000>----------------------------------------------</font></p>
	
	<h2><p align=center>Register a new account</p></h2>
		
	<form method=post action=register.php>
	<table bordercolor=white align=center>
	<tr>
	<th><p align=right>Account:</p></th>   
	<th><input type=text     name='newuser'/></th>
	<tr>
	<th><p align=right>Password:</p></th>
	<th><input type=password name='newuser_pw'/></th>
	<tr>
	<th><p align=right>Password again:</p></th>
	<th><input type=password name='newuser_pw2'/></th>
	<tr>
	<th><p align=right>Name:</p></th>
	<th><input type=text     name='newuser_name'/></th>
	<tr>
	<th><p align=right>Email:</p></th>
	<th><input type=text     name='newuser_email'/></th>
	<tr>
	<th><p align=right>Birthday:</p><th>
	<input type=text         name='newuser_birth'/></th>
	<tr>
	<th><p align=right>Sex:</p></th>
	<th><input type=radio value='M' name='newuser_sex'/>Male
            <input type=radio value='F' name='newuser_sex'/>Female</th>
	</table>
	<p align=center><input type='submit'/></p>
	<form method=post action=register.php>";
    }
    else
    {

	$host    ="localhost";
	$username="dbuser";
	$password="dbuser";
	$database="db_pj3";
	$con=mysqli_connect($host, $username, $password, $database);
	
	if(isset($_POST["newuser"])
	&& isset($_POST["newuser_pw"])
	&& isset($_POST["newuser_pw2"])
        && isset($_POST["newuser_name"])
	&& isset($_POST["newuser_email"]) 
	&& isset($_POST["newuser_birth"])
	&& isset($_POST["newuser_sex"]))
	{
            $sql=
            "SELECT uid
             FROM   users
             WHERE  uid='$_POST[newuser]'";

            $existed=mysqli_query($con, $sql);
            
            if($row=mysqli_fetch_array($existed))
            {
                echo
                "
                    <p align=center><font color=#ff8000>----------------------------------------------</font></p>
                <h1><p align=center><font color=#ff0000>                 <b>FACENOTE</b>              </font></p></h1>
                    <p align=center><font color=#ff8000>----------------------------------------------</font></p>

                <h2><p align='center'><font color=blue>the account has been used!</font></p></h2>
                <form method=post action=register.php>		
                <table bordercolor=white align=center>
                <tr>
                <th><p align=right>Account:</p></th>   
                <th><input type=text     name='newuser'/></th>
                <tr>
                <th><p align=right>Password:</p></th>
                <th><input type=password name='newuser_pw'/></th>
                <tr>
                <th><p align=right>Password again:</p></th>
                <th><input type=password name='newuser_pw2'/></th>
                <tr>
                <th><p align=right>Name:</p></th>
                <th><input type=text     name='newuser_name'/></th>
                <tr>
                <th><p align=right>Email:</p></th>
                <th><input type=text     name='newuser_email'/></th>
                <tr>
                <th><p align=right>Birthday:</p><th>
                <input type=text         name='newuser_birth'/></th>
                <tr>
                <th><p align=right>Sex:</p></th>
                <th><input type=radio value='M' name='newuser_sex'/>Male
                    <input type=radio value='F' name='newuser_sex'/>Female</th>
                </table>
                <p align=center><input type='submit'/></p>		
                <form method=post action=register.php>"; 
            }
           else
            {   
                if($_POST["newuser_pw"]==$_POST["newuser_pw2"])
		{
                    $sql=
                    "INSERT INTO users
                     (uid, password, name, email, birthday, sex)
                     VALUES ('$_POST[newuser]', md5('$_POST[newuser_pw]'), '$_POST[newuser_name]',
                             '$_POST[newuser_email]', '$_POST[newuser_birth]', '$_POST[newuser_sex]')";
                    mysqli_query($con, $sql);
                    echo 
                    "
                        <p align=center><font color=#ff8000>----------------------------------------------</font></p>
                    <h1><p align=center><font color=#ff0000>                 <b>FACENOTE</b>              </font></p></h1>
                        <p align=center><font color=#ff8000>----------------------------------------------</font></p>

                    <h2><p align='center'><font color=blue>Registe Success!</font></p></h2>
                    <h3><p align='center'><a href=index.php>Login</a></p></h3>";
                }
                else
                {
                    echo
                    "
                        <p align=center><font color=#ff8000>----------------------------------------------</font></p>
                    <h1><p align=center><font color=#ff0000>                 <b>FACENOTE</b>              </font></p></h1>
                        <p align=center><font color=#ff8000>----------------------------------------------</font></p>

                    <h2><p align='center'><font color=blue>check password again!</font></p></h2>
					
                    <form method=post action=register.php>
                    <table bordercolor=white align=center>
                    <tr>
                    <th><p align=right>Account:</p></th>   
                    <th><input type=text     name=newuser/></th>
                    <tr>
                    <th><p align=right>Password:</p></th>
                    <th><input type=password name=newuser_pw/></th>
                    <tr>
                    <th><p align=right>Password again:</p></th>
                    <th><input type=password name=newuser_pw2/></th>
                    <tr>
                    <th><p align=right>Name:</p></th>
                    <th><input type=text     name=newuser_name/></th>
                    <tr>
                    <th><p align=right>Email:</p></th>
                    <th><input type=text     name=newuser_email/></th>
                    <tr>
                    <th><p align=right>Birthday:</p><th>
                    <input type=text         name=newuser_birth/></th>
                    <tr>
                    <th><p align=right>Sex:</p></th>
                    <th><input type=radio value='M' name=newuser_sex/>Male
                    <input type=radio value='F' name=newuser_sex/>Female</th>
                    </table>
                    <p align=center><input type='submit'/></p>
                    <form method=post action=register.php>";
                    
                }
            }
        }
        else
        {
            echo
            "
                <p align=center><font color=#ff8000>----------------------------------------------</font></p>
            <h1><p align=center><font color=#ff0000>                 <b>FACENOTE</b>              </font></p></h1>
                <p align=center><font color=#ff8000>----------------------------------------------</font></p>

            <h2><p align='center'><font color=blue>not finish!</font></p></h2>
            <form method=post action=register.php>		
            <table bordercolor=white align=center>
            <tr>
            <th><p align=right>Account:</p></th>   
            <th><input type=text     name='newuser'/></th>
            <tr>
            <th><p align=right>Password:</p></th>
            <th><input type=password name='newuser_pw'/></th>
            <tr>
            <th><p align=right>Password again:</p></th>
            <th><input type=password name='newuser_pw2'/></th>
            <tr>
            <th><p align=right>Name:</p></th>
            <th><input type=text     name='newuser_name'/></th>
            <tr>
            <th><p align=right>Email:</p></th>
            <th><input type=text     name='newuser_email'/></th>
            <tr>
            <th><p align=right>Birthday:</p><th>
            <input type=text         name='newuser_birth'/></th>
            <tr>
            <th><p align=right>Sex:</p></th>
            <th><input type=radio value='M' name='newuser_sex'/>Male
                <input type=radio value='F' name='newuser_sex'/>Female</th>
            </table>
            <p align=center><input type='submit'/></p>		
            <form method=post action=register.php>";
        }
        
        mysqli_close($con);
    }

?>
</body>

</html>