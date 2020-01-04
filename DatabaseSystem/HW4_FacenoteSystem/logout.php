<!DOCTYPE html>
<?php
	session_start();
	session_destroy();
 ?> 
<html>

<head>
</head>

<body>
<?php
        $host    ="localhost";
	$username="db_user";
	$password="db_user";
	$database="db_0016046";
	$con=mysqli_connect($host, $username, $password, $database);
        
        echo
            "<p align=center><font color=#ff8000>----------------------------------------------</font></p>
	<h1><p align=center><font color=#ff0000>                 <b>FACENOTE</b>              </font></p></h1>
	    <p align=center><font color=#ff8000>----------------------------------------------</font></p>
	
        <h1><p align=center><font color=#ff8000>Goodbye!</font></p></h1>";
        if(isset($_GET['d']) && $_GET['d']==1)
        {
               $sql=
               "DELETE FROM friends 
                WHERE uid='$_GET[duser]' OR friend_id='$_GET[duser]'";
                mysqli_query($con, $sql);
               $sql=
               "DELETE likes
                FROM   likes, articles 
                WHERE  (likes.uid='$_GET[duser]') OR (likes.postid=articles.postid AND articles.uid='$_GET[duser]')";
                mysqli_query($con, $sql);
                $sql=
               "DELETE FROM articles 
                WHERE uid='$_GET[duser]'";
                mysqli_query($con, $sql);
               $sql=
               "DELETE FROM users 
                WHERE uid='$_GET[duser]'";
                mysqli_query($con, $sql);
                
                echo "<h3><font color=blue><p align=center>You have deleted your account!</p></font></h3>";
        }
        
        echo
            "<h3><p align=center><a href=index.php>Login</a></p></h3>
            <br/>";
                    
        mysqli_close($con);
?>
</body>

</html>