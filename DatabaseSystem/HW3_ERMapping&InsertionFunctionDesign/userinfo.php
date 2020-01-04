<!DOCTYPE html>
<?php
	session_start();
?> 
<html>

<head>
</head>

<body>
<?php
	$host    ="localhost";
	$username="dbuser";
	$password="dbuser";
	$database="db_pj3";
	$con=mysqli_connect($host, $username, $password, $database);
	
	if(!isset($_SESSION['login_user']))
	{	
            die ("<p align=center><font color=#ff8000>----------------------------------------------</font></p>
              <h1><p align=center><font color=#ff0000>                 <b>FACENOTE</b>              </font></p></h1>
	          <p align=center><font color=#ff8000>----------------------------------------------</font></p><h2>
                  <p align='center'><font color=blue>You have not logged in yet!</font></p></h2>
                  <h3><p align='center'><a href=index.php>Login</a></p></h3>");
	}
	else
	{
            if(isset($_GET['newfriend']))
            {   
                $_GET['n']=$_SESSION['login_user'];
                $addfail=0;
                $sql=
                "SELECT uid
                 FROM   users
                 WHERE  uid='$_GET[newfriend]'";
                                     
                $existed=mysqli_query($con, $sql);
                if($row=mysqli_fetch_array($existed))
                {   
                    $sql=
                    "SELECT uid, friend_id
                     FROM   friends
                     WHERE  uid='$_SESSION[login_user]' AND friend_id='$_GET[newfriend]'";
                    $existedf=mysqli_query($con, $sql);
                    if($row=mysqli_fetch_array($existedf))
                    {
                        $addfail=2;
                    }
                    else 
                    {
                        $sql=
                        "INSERT INTO friends 
                         (uid, friend_id, relationship)
                         VALUES ('$_SESSION[login_user]','$_GET[newfriend]', '$_GET[relation]')";
                        mysqli_query($con, $sql);
                        $sql=
                        "INSERT INTO friends 
                         (uid, friend_id, relationship)
                         VALUES ('$_GET[newfriend]', '$_SESSION[login_user]', '$_GET[relation]')";
                        mysqli_query($con, $sql);
                    }
                }
                else
                {
                    $addfail=1;
                }
            }
            $sql=
            "SELECT name 
             FROM   users 
             WHERE  uid     ='".mysqli_real_escape_string($con, $_SESSION['login_user'])."' 
	     AND    password='".mysqli_real_escape_string($con, md5($_SESSION['login_pw']))."' ";
            
            $user=mysqli_query($con, $sql);
	
            if($row=mysqli_fetch_array($user))
            {
                echo
                "<table bordercolor=white align=center width=900>
                 <tr>
                 <th bgcolor=orange colspan=3 align=left>
                 <h2><font style='background-color:orange'> &nbsp Welcome <i>".$row['name']."</i> !</font></h2>
                     &nbsp <a href=main.php>Main Page</a>
                     &nbsp <a href=userinfo.php?n=$_SESSION[login_user]>User Info</a>
                     &nbsp <a href=logout.php>Log Out</a>
                 </th>
                 <tr><th colspan=3><hr></th>
                 <tr><th align=center valign=top width=500>";
                

                    $sql=
                    "SELECT name, uid, email, birthday
                     FROM   users 
                     WHERE  users.uid='$_GET[n]'";

                
                $userinfo=mysqli_query($con, $sql);
	
                if($row=mysqli_fetch_array($userinfo))
                {
                    echo 
                    "<table bordercolor=white align=left width=500>
                        <tr><th colspan=2 align=center><h2>$row[name]</h2></th>
                        <tr><th align=right width=200>ID :      </th><th align=left>$row[uid]</th>
                        <tr><th align=right>Email :   </th><th align=left>$row[email]</th>
                        <tr><th align=right>Birthday :</th><th align=left>$row[birthday]</th>
                     
                        <tr><th colspan=2 align=center><h2>Friend</h2></th>";
                            
                     $sql=
                    "SELECT users.uid, users.name 
                     FROM   users, friends
                     WHERE  friends.uid='$_SESSION[login_user]'
                     AND    friends.friend_id=users.uid";
                
                    $friend=mysqli_query($con, $sql);
                    while($row=mysqli_fetch_array($friend))
                    {
                       echo "<tr><th colspan=2><a href=userinfo.php?n=$row[uid]>$row[name]</a></th>";
                    }
                    echo
                    "</table>
                     <th valign=top>
                     <table bordercolor=white align=left>
                     <tr><th colspan=2><h2> &nbsp </h2></th>
                     <tr><th colspan=2>&nbsp</th>
                     <tr><th colspan=2>&nbsp</th>
                     <tr><th colspan=2>&nbsp</th>
                     <form method=get action=userinfo.php?n='$_SESSION[login_user]'>
                     <tr><th colspan=2><h2>Add a new friend</h2></th>";
                     if(isset($addfail))
                     {
                         if($addfail==0)
                            echo "<tr><th colspan=2><h3><font color=blue>success!</font></h3></th>";
                         if($addfail==1)
                            echo "<tr><th colspan=2><h3><font color=blue>the account is not existed!</font></h3></th>";
                         if($addfail==2)
                            echo "<tr><th colspan=2><h3><font color=blue>you are already friends!</font></h3></th>";
                     }
                     echo
                     "<tr><th align=right>Friend ID:  </th><th><input type=text     name='newfriend'></th>
                      <tr><th align=left>Relationship:</th><th><input type=text     name='relation'></th>
                      
                      <tr><th colspan=2><input type=submit></th>
                      </form></table>";
                }
            }
	
		
		mysqli_close($con);
	}
?>
	
</body>

</html>