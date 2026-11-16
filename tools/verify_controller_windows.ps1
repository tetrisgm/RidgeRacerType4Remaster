# Test a running isolated Windows runtime at a saved race grid. The opt-in
# debug server and SDL virtual pad exercise actual device assignment/polling.
param([Parameter(Mandatory=$true)][string]$Root,[int]$Port=4797)
$ErrorActionPreference='Stop'
function C($o) {
 $tcp=[Net.Sockets.TcpClient]::new('127.0.0.1',$Port);$tcp.ReceiveTimeout=15000
 try {$s=$tcp.GetStream();$w=[IO.StreamWriter]::new($s);$w.AutoFlush=$true;$r=[IO.StreamReader]::new($s);$w.WriteLine(($o|ConvertTo-Json -Compress));$v=$r.ReadLine()|ConvertFrom-Json;return $v}finally{$tcp.Close()}
}

function Check($condition,$message){if(-not $condition){throw $message}}
function V($buttons=0,$rx=0,$ry=0){$r=C @{cmd='virtual_pad';select=1;buttons=$buttons;rx=$rx;ry=$ry};Check $r.ok 'virtual pad failed'}
function Shot($name){$p=(Join-Path $Root ('evidence\'+$name+'.png'));$n=(C @{cmd='present_shot_seq'}).seq;C @{cmd='present_shot';path=$p}|Out-Null;for($j=0;$j-lt100;$j++){Start-Sleep -Milliseconds 20;$shot=C @{cmd='present_shot_seq'};if($shot.seq-ne$n){Check $shot.wrote 'capture failed';return}};throw 'capture timeout'}
New-Item -ItemType Directory -Force (Join-Path $Root 'evidence') | Out-Null
$r=@{before=(C @{cmd='controller_status'})}
try {
C @{cmd='turbo';enabled=0}|Out-Null
$g=(C @{cmd='savestate_status'}).generation;C @{cmd='savestate';op='load';slot=0}|Out-Null
for($i=0;$i-lt100;$i++){Start-Sleep -Milliseconds 50;$s=C @{cmd='savestate_status'};if($s.generation-ne$g){break}}
Check $s.last_ok 'grid failed'
C @{cmd='display_aspect';num=16;den=9}|Out-Null
V;Start-Sleep 3
$r.virtual=C @{cmd='controller_status'};Check $r.virtual.attached 'virtual not assigned'
$r.rewinds=@()
for($i=0;$i-lt3;$i++) {
 $before=C @{cmd='rewind_status'};V 8;Start-Sleep -Milliseconds 400
 $held=C @{cmd='controller_status'};$rew=C @{cmd='rewind_status'}
 Check ($held.rewind_held -and $rew.direct) 'Y did not start hold rewind'
 V;Start-Sleep -Milliseconds 350
 $after=C @{cmd='rewind_status'};Check (-not$after.open -and $after.release_us-gt0) 'Y release failed'
 Check ($after.release_target-lt$before.tick) 'rewind did not go back'
 $r.rewinds+=@{held=$held;before=$before;rewind=$rew;after=$after};Start-Sleep 2
}
$r.camera=@()
foreach($pose in @(@('right',32767,0),@('rear',0,32767),@('left',-32768,0),@('forward',0,0))) {
 V 0 $pose[1] $pose[2];Start-Sleep -Milliseconds 250
 $samples=@()
 for($i=0;$i-lt8;$i++) {$samples+=@{controller=(C @{cmd='controller_status'});yaw=(C @{cmd='mem_words';addr='0x1F80001C';count=1})};Shot ($pose[0]+'-'+$i)}
 $angles=@($samples | ForEach-Object {$_.yaw.words[0]} | Select-Object -Unique)
 Check ($angles.Count-eq1) ('held camera moved: '+$pose[0])
 $r.camera+=@{pose=$pose[0];samples=$samples}
}
V 16;Start-Sleep -Milliseconds 150;V;Start-Sleep -Milliseconds 150
$r.menu=C @{cmd='menu_host'};Check $r.menu.open 'Back menu failed'
V 16;Start-Sleep -Milliseconds 150;V;Start-Sleep -Milliseconds 150
$r.menu_closed=C @{cmd='menu_host'};Check (-not$r.menu_closed.open) 'Back close failed'
C @{cmd='virtual_pad';connected=0}|Out-Null;Start-Sleep -Milliseconds 150
$r.restored=C @{cmd='controller_status'}
$r.dispatch=C @{cmd='dispatch_stats'};Check ($r.dispatch.miss_total-eq0 -and $r.dispatch.segment_miss_total-eq0) 'dispatch miss'
$r.passed=$true
} finally {
 C @{cmd='virtual_pad';connected=0}|Out-Null
 $r|ConvertTo-Json -Depth 18|Set-Content (Join-Path $Root 'evidence\virtual-check.json')
}
$r|ConvertTo-Json -Depth 18|Set-Content (Join-Path $Root 'evidence\virtual-check.json')
$r|ConvertTo-Json -Depth 18 -Compress
