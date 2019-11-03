Here's a example for PHP connection with RPC client:

```
<?php 

 //rpc connection
 $rpcconn = 'http://rpc_user:rpc_password@127.0.0.1:rpc_port/';
 
 //mount request
 function newRequest($method, array $params = array())
 {
     $req = array(
         'jsonrpc' => '1.0',
         'id' => "idTest",
         'method' => $method
     );
 
     $req['params'] = $params;
     $json = json_encode($req);
     return $json;
 }

 //send rpc command
 function rpcCall($js) {
     global $rpcconn;

     $ch = curl_init();
     curl_setopt($ch, CURLOPT_POST, 1);
     curl_setopt($ch, CURLOPT_HEADER, 0);
     curl_setopt($ch, CURLOPT_TIMEOUT, 5);
     curl_setopt($ch, CURLOPT_URL, $rpcconn);
     curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE);
     curl_setopt($ch, CURLOPT_POSTFIELDS, $js);
     
     $result = curl_exec($ch);

     curl_close($ch);
     return $result;
}

//example
$req = newRequest('getinfo');
$res = rpcCall($req);

echo $res;

?>
```
