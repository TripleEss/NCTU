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
	$username="db_user";
	$password="db_user";
	$database="db_0016046";
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
            // add a new friend
            if(isset($_GET['newfriend']))
            {   
                $_GET['n']=$_SESSION['login_user'];
                $addfail=2;
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
                    if(!$row=mysqli_fetch_array($existedf))
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
                        
                        $addfail=0;
                    }
                }
                else
                {
                    $addfail=1;
                }
                
                unset($_GET['addfriend']);
            }
            
            // delete a friend
            if(isset($_GET['delfriend']))
            {   
                //echo $_GET['delfriend'].$_SESSION['login_user'];
                $_GET['n']=$_SESSION['login_user'];
                $delfail=2;
                $sql=
                "SELECT uid
                 FROM   users
                 WHERE  uid='$_GET[delfriend]'";
                                     
                $existed=mysqli_query($con, $sql);
                if($row=mysqli_fetch_array($existed))
                {   
                    $sql=
                    "SELECT uid, friend_id
                     FROM   friends
                     WHERE  uid='$_SESSION[login_user]' AND friend_id='$_GET[delfriend]'";
                    $existedf=mysqli_query($con, $sql);
                    while($row=mysqli_fetch_array($existedf))
                    {
                        $sql=
                        "DELETE FROM friends 
                         WHERE uid='$_SESSION[login_user]' AND friend_id='$_GET[delfriend]'";
                        mysqli_query($con, $sql);
                        $sql=
                        "DELETE FROM friends 
                         WHERE riend_id='$_SESSION[login_user]' AND uid='$_GET[delfriend]'";
                        mysqli_query($con, $sql);
                        
                        $delfail=0;
                    }
                }
                else
                {
                    $delfail=1;
                }
                
                unset($_GET['delfriend']);
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
                 <th bgcolor=orange colspan=2 align=left>
                 <h2><font style='background-color:orange'> &nbsp Welcome <i>".$row['name']."</i> !</font></h2>  
                     &nbsp <a href=main.php>Main Page</a>
                     &nbsp <a href=userinfo.php?n=$_SESSION[login_user]>User Info</a>
                     &nbsp <a href=logout.php?d=0>Log Out</a>
                     &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp
                     &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp
                     &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp
                     &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp
                     &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp <right><a href=logout.php?d=1&duser=$_SESSION[login_user]>Delete Account</a></right>
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
                        <tr><th align=right>ID :      </th><th align=left width=300>$row[uid]</th>
                        <tr><th align=right>Email :   </th><th align=left width=300>$row[email]</th>
                        <tr><th align=right>Birthday :</th><th align=left width=300>$row[birthday]</th>";
                    
                    
                     
                     echo "<tr><th colspan=2 align=center><h2>Friend</h2></th>";
                            
                     $sql=
                    "SELECT users.uid, users.name 
                     FROM   users, friends
                     WHERE  friends.uid='$_SESSION[login_user]'
                     AND    friends.friend_id=users.uid";
                
                    $friend=mysqli_query($con, $sql);
                    while($row=mysqli_fetch_array($friend))
                    {
                        echo "<tr><th width=300 align=right>
                              <a href=userinfo.php?n=$row[uid]>$row[name]</a></th>";
                        
                        if($_GET['n']!=$_SESSION['login_user'])
                        {   
                             $sql0=
                             "SELECT T1.name
                              FROM
                                  (SELECT friend_id, name 
                                   FROM   friends, users
                                   WHERE  friends.uid='$_SESSION[login_user]' AND users.uid=friends.friend_id) AS T1
                              JOIN
                                  (SELECT friend_id, name 
                                   FROM   friends, users
                                   WHERE  friends.uid='$_GET[n]' AND users.uid=friends.friend_id) AS T2
                              ON T1.friend_id=T2.friend_id";

                             $cofriend=mysqli_query($con, $sql0);

                             while($row0=mysqli_fetch_array($cofriend))
                             {
                                    if($row['name']==$row0['name'])
                                        echo "<th align=left><font color=red> ( common ) </font></th>";
                             }

                             echo "</th>";
                         }
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
                            echo "<tr><th colspan=2><h3><font color=blue>Success!</font></h3></th>";
                         if($addfail==1)
                            echo "<tr><th colspan=2><h3><font color=blue>The account is not existed!</font></h3></th>";
                         if($addfail==2)
                            echo "<tr><th colspan=2><h3><font color=blue>You are already friends!</font></h3></th>";
                     }
                     echo
                     "<tr><th align=right>Friend ID:  </th><th><input type=text     name='newfriend'></th>
                      <tr><th align=left>Relationship:</th><th><input type=text     name='relation'></th>
                      
                      <tr><th colspan=2><input type=submit></th>
                      </form>";
                     
                     echo 
                     "<form method=get action=userinfo.php?n='$_SESSION[login_user]'>
                      <tr><th colspan=2><h2>Delete a friend</h2></th>";
                     if(isset($delfail))
                     {
                         if($delfail==0)
                            echo "<tr><th colspan=2><h3><font color=blue>Success!</font></h3></th>";
                         if($delfail==1)
                            echo "<tr><th colspan=2><h3><font color=blue>The account is not existed!</font></h3></th>";
                         if($delfail==2)
                            echo "<tr><th colspan=2><h3><font color=blue>You have no friend with such ID!</font></h3></th>";
                     }
                     
                      echo
                     "<tr><th align=right>Friend ID:  </th><th><input type=text     name='delfriend'></th>                   
                      <tr><th colspan=2><input type=submit></th>
                      </form></table>";
                }
            }
	
		
		mysqli_close($con);
	}
?>
	
</body>

</html>