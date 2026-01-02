🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥
                                                                                             
 ██    ██ █ ██    █ █ █████ █   █ █████ █     █       ██████ █████ █████ ██████ █████ █████  
 ███  ███ █ ███   █ █ █     █   █ █     █     █         ██   █     █       ██   █     █   ██ 
 █ ████ █ █ █ ██  █ █ █████ █████ ████  █     █         ██   ████  █████   ██   ████  █████  
 █  ██  █ █ █  ██ █ █     █ █   █ █     █     █         ██   █         █   ██   █     █   ██ 
 █      █ █ █   ███ █ █████ █   █ █████ █████ █████     ██   █████ █████   ██   █████ █   ██ 
                                                                                             
🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥
                                                                                              
———————————— builtins
Test   1: ✅ echo hello world 
Test   2: ✅ echo "hello world" 
Test   3: ✅ echo 'hello world' 
Test   4: ✅ echo hello'world' 
Test   5: ✅ echo hello""world 
Test   6: ✅ echo '' 
Test   7: ✅ echo "$PWD" 
Test   8: ✅ echo '$PWD' 
Test   9: ✅ echo "aspas ->'" 
Test  10: ✅ echo "aspas -> ' " 
Test  11: ✅ echo 'aspas ->"' 
Test  12: ✅ echo 'aspas -> " ' 
Test  13: ✅ echo "> >> < * ? [ ] | ; [ ] || && ( ) & # $  <<" 
Test  14: ✅ echo '> >> < * ? [ ] | ; [ ] || && ( ) & # $  <<' 
Test  15: ✅ echo "exit_code ->$? user ->$USER home -> $HOME" 
Test  16: ✅ echo 'exit_code ->$? user ->$USER home -> $HOME' 
Test  17: ✅ echo "$" 
Test  18: ✅ echo '$' 
Test  19: ✅ echo $ 
Test  20: ✅ echo $? 
Test  21: ✅ echo $?HELLO 
Test  22: ✅ pwd 
Test  23: ✅ pwd oi 
Test  24: ✅ export hello 
Test  25: ✅ export HELLO=123 
Test  26: ✅ export A- 
Test  27: ✅ export HELLO=123 A 
Test  28: ✅ export HELLO="123 A-" 
Test  29: ✅ export hello world 
Test  30: ✅ export HELLO-=123 
Test  31: ✅ export = 
Test  32: ✅ export 123 
Test  33: ✅ unset 
Test  34: ✅ unset HELLO 
Test  35: ✅ unset HELLO1 HELLO2 
Test  36: ✅ unset HOME 
Test  37: ✅ unset PATH 
Test  38: ✅ unset SHELL 
Test  39: ✅ cd $PWD 
Test  40: ✅ cd $PWD hi 
Test  41: ✅ cd 123123 
Test  42: ✅ exit 123 
Test  43: ✅ exit 298 
Test  44: ✅ exit +100 
Test  45: ✅ exit "+100" 
Test  46: ✅ exit +"100" 
Test  47: ✅ exit -100 
Test  48: ✅ exit "-100" 
Test  49: ✅ exit -"100" 
Test  50: ✅ exit hello 
Test  51: ✅ exit 42 world 
Test  52: ✅  
———————————— pipes
Test  53: ✅ env | sort | grep -v SHLVL | grep -v ^_ 
Test  54: ✅ cat ./test_files/infile_big | grep oi 
Test  55: ✅ cat minishell.h | grep ");"$ 
Test  56: ✅ export GHOST=123 | env | grep GHOST 
———————————— redirects
Test  57: ✅ grep hi <./test_files/infile 
Test  58: ✅ grep hi "<infile" <         ./test_files/infile 
Test  59: ✅ echo hi < ./test_files/infile bye bye 
Test  60: ✅ grep hi <./test_files/infile_big <./test_files/infile 
Test  61: ✅ echo <"./test_files/infile" "bonjour       42" 
Test  62: ✅ cat <"./test_files/file name with spaces" 
Test  63: ✅ cat <./test_files/infile_big ./test_files/infile 
Test  64: ✅ cat <"1""2""3""4""5" 
Test  65: ✅ echo <"./test_files/infile" <missing <"./test_files/infile" 
Test  66: ✅ echo <missing <"./test_files/infile" <missing 
Test  67: ✅ cat <"./test_files/infile" 
Test  68: ✅ echo <"./test_files/infile_big" | cat <"./test_files/infile" 
Test  69: ✅ echo <"./test_files/infile_big" | cat "./test_files/infile" 
Test  70: ✅ echo <"./test_files/infile_big" | echo <"./test_files/infile" 
Test  71: ✅ echo hi | cat <"./test_files/infile" 
Test  72: ✅ echo hi | cat "./test_files/infile" 
Test  73: ✅ cat <"./test_files/infile" | echo hi 
Test  74: ✅ cat <"./test_files/infile" | grep hello 
Test  75: ✅ cat <"./test_files/infile_big" | echo hi 
Test  76: ✅ cat <missing 
Test  77: ✅ cat <missing | cat 
Test  78: ✅ cat <missing | echo oi 
Test  79: ✅ cat <missing | cat <"./test_files/infile" 
Test  80: ✅ echo <123 <456 hi | echo 42 
Test  81: ✅ ls >./outfiles/outfile01 
Test  82: ✅ ls >         ./outfiles/outfile01 
Test  83: ✅ echo hi >         ./outfiles/outfile01 bye 
Test  84: ✅ ls >./outfiles/outfile01 >./outfiles/outfile02 
Test  85: ✅ ls >./outfiles/outfile01 >./test_files/invalid_permission 
Test  86: ✅ ls >"./outfiles/outfile with spaces" 
Test  87: ✅ ls >"./outfiles/outfile""1""2""3""4""5" 
Test  88: ✅ ls >"./outfiles/outfile01" >./test_files/invalid_permission >"./outfiles/outfile02" 
Test  89: ✅ ls >./test_files/invalid_permission >"./outfiles/outfile01" >./test_files/invalid_permission 
Test  90: ✅ cat <"./test_files/infile" >"./outfiles/outfile01" 
Test  91: ✅ echo hi >./outfiles/outfile01 | echo bye 
Test  92: ✅ echo hi >./outfiles/outfile01 >./outfiles/outfile02 | echo bye 
Test  93: ✅ echo hi | echo >./outfiles/outfile01 bye 
Test  94: ✅ echo hi | echo bye >./outfiles/outfile01 >./outfiles/outfile02 
Test  95: ✅ echo hi >./outfiles/outfile01 | echo bye >./outfiles/outfile02 
Test  96: ✅ echo hi >./outfiles/outfile01 >./test_files/invalid_permission | echo bye 
Test  97: ✅ echo hi >./test_files/invalid_permission | echo bye 
Test  98: ✅ echo hi >./test_files/invalid_permission >./outfiles/outfile01 | echo bye 
Test  99: ✅ echo hi | echo bye >./test_files/invalid_permission 
Test 100: ✅ echo hi | >./outfiles/outfile01 echo bye >./test_files/invalid_permission 
Test 101: ✅ echo hi | echo bye >./test_files/invalid_permission >./outfiles/outfile01 
Test 102: ✅ cat <"./test_files/infile" >./test_files/invalid_permission 
Test 103: ✅ cat >./test_files/invalid_permission <"./test_files/infile" 
Test 104: ✅ cat <missing >./outfiles/outfile01 
Test 105: ✅ cat >./outfiles/outfile01 <missing 
Test 106: ✅ cat <missing >./test_files/invalid_permission 
Test 107: ✅ cat >./test_files/invalid_permission <missing 
Test 108: ✅ cat >./outfiles/outfile01 <missing >./test_files/invalid_permission 
Test 109: ✅ ls >>./outfiles/outfile01 
Test 110: ✅ ls >>      ./outfiles/outfile01 
Test 111: ✅ ls >>./outfiles/outfile01 >./outfiles/outfile01 
Test 112: ✅ ls >./outfiles/outfile01 >>./outfiles/outfile01 
Test 113: ✅ ls >./outfiles/outfile01 >>./outfiles/outfile01 >./outfiles/outfile02 
Test 114: ✅ ls >>./outfiles/outfile01 >>./outfiles/outfile02 
Test 115: ✅ ls >>./test_files/invalid_permission 
Test 116: ✅ ls >>./test_files/invalid_permission >>./outfiles/outfile01 
Test 117: ✅ ls >>./outfiles/outfile01 >>./test_files/invalid_permission 
Test 118: ✅ ls >./outfiles/outfile01 >>./test_files/invalid_permission >>./outfiles/outfile02 
Test 119: ✅ ls <missing >>./test_files/invalid_permission >>./outfiles/outfile02 
Test 120: ✅ ls >>./test_files/invalid_permission >>./outfiles/outfile02 <missing 
Test 121: ✅ echo hi >>./outfiles/outfile01 | echo bye 
Test 122: ✅ echo hi >>./outfiles/outfile01 >>./outfiles/outfile02 | echo bye 
Test 123: ✅ echo hi | echo >>./outfiles/outfile01 bye 
Test 124: ✅ echo hi | echo bye >>./outfiles/outfile01 >>./outfiles/outfile02 
Test 125: ✅ echo hi >>./outfiles/outfile01 | echo bye >>./outfiles/outfile02 
Test 126: ✅ echo hi >>./test_files/invalid_permission | echo bye 
Test 127: ✅ echo hi >>./test_files/invalid_permission >./outfiles/outfile01 | echo bye 
Test 128: ✅ echo hi | echo bye >>./test_files/invalid_permission 
Test 129: ✅ echo hi | echo >>./outfiles/outfile01 bye >./test_files/invalid_permission 
Test 130: ✅ cat <minishell.h>./outfiles/outfile 
Test 131: ✅ cat <minishell.h|ls 
———————————— extras
Test 132: ✅  
Test 133: ✅ $PWD 
Test 134: ✅ $EMPTY 
Test 135: ✅ $EMPTY echo hi 
Test 136: ✅⚠️  ./test_files/invalid_permission 
mini error = ()
bash error = ( Permission denied)
Test 137: ✅⚠️  ./missing.out 
mini error = ()
bash error = ( No such file or directory)
Test 138: ✅ missing.out 
Test 139: ✅ "aaa" 
Test 140: ✅ test_files 
Test 141: ✅ ./test_files 
Test 142: ✅⚠️  /test_files 
mini error = ()
bash error = ( No such file or directory)
Test 143: ✅ minishell.h 
Test 144: ✅ $ 
Test 145: ✅ $? 
Test 146: ✅⚠️  README.md 
mini error = ()
bash error = ( Permission denied)
146/146
🎊 🎊 🎊
😎 😎 😎
🎉 🎉 🎉



 minishell_tester git:(main) ✗ ./tester syntax
🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥
                                                                                             
 ██    ██ █ ██    █ █ █████ █   █ █████ █     █       ██████ █████ █████ ██████ █████ █████  
 ███  ███ █ ███   █ █ █     █   █ █     █     █         ██   █     █       ██   █     █   ██ 
 █ ████ █ █ █ ██  █ █ █████ █████ ████  █     █         ██   ████  █████   ██   ████  █████  
 █  ██  █ █ █  ██ █ █     █ █   █ █     █     █         ██   █         █   ██   █     █   ██ 
 █      █ █ █   ███ █ █████ █   █ █████ █████ █████     ██   █████ █████   ██   █████ █   ██ 
                                                                                             
🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥
                                                                                              
———————————— syntax
Test   1: ❌ | 
mini exit code = 258
bash exit code = 258
Test   2: ❌ | echo oi 
mini exit code = 258
bash exit code = 258
Test   3: ❌ | | 
mini exit code = 258
bash exit code = 258
Test   4: ❌ | $ 
mini exit code = 258
bash exit code = 258
Test   5: ❌ | > 
mini exit code = 258
bash exit code = 258
Test   6: ❌ > 
mini exit code = 258
bash exit code = 258
Test   7: ❌ >> 
mini exit code = 258
bash exit code = 258
Test   8: ❌ >>> 
mini exit code = 258
bash exit code = 258
Test   9: ❌ < 
mini exit code = 258
bash exit code = 2
Test  10: ❌ << 
mini exit code = 258
bash exit code = 258
Test  11: ❌ echo hi < 
mini exit code = 258
bash exit code = 258
Test  12: ❌ cat    <| ls 
mini exit code = 258
bash exit code = 258
Test  13: ❌ echo hi | > 
mini exit code = 258
bash exit code = 258
Test  14: ❌ echo hi | > >> 
mini exit code = 258
bash exit code = 258
mini error = ( syntax error near unexpected token `>')
bash error = ( syntax error near unexpected token `>>')
Test  15: ❌ echo hi | < | 
mini exit code = 258
bash exit code = 258
Test  16: ❌ echo hi |   | 
mini exit code = 258
bash exit code = 258
Test  17: ✅ echo hi |  "|" 
1/17
😭 😭 😭



➜  minishell_tester git:(main) ✗ ./tester os_specific
🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥
                                                                                             
 ██    ██ █ ██    █ █ █████ █   █ █████ █     █       ██████ █████ █████ ██████ █████ █████  
 ███  ███ █ ███   █ █ █     █   █ █     █     █         ██   █     █       ██   █     █   ██ 
 █ ████ █ █ █ ██  █ █ █████ █████ ████  █     █         ██   ████  █████   ██   ████  █████  
 █  ██  █ █ █  ██ █ █     █ █   █ █     █     █         ██   █         █   ██   █     █   ██ 
 █      █ █ █   ███ █ █████ █   █ █████ █████ █████     ██   █████ █████   ██   █████ █   ██ 
                                                                                             
🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥
                                                                                              
———————————— os_specific
Test   1: ❌ unset HELLO= 
mini exit code = 0
bash exit code = 1
mini error = ()
bash error = ( not a valid identifier)
Test   2: ❌ unset  A- 
mini exit code = 0
bash exit code = 1
mini error = ()
bash error = ( not a valid identifier)
Test   3: ✅ export HELLO=123 A- WORLD=456 
Test   4: ❌ unset  HELLO A- WORLD 
mini exit code = 0
bash exit code = 1
mini error = ()
bash error = ( not a valid identifier)
Test   5: ❌ export UNO=1 DOS-2 TRES=3 || env | grep TRES 
mini output = ()
bash output = (TRES=3)
mini exit code = 1
bash exit code = 0
Test   6: ❌ export | sort | grep -v SHLVL | grep -v "declare -x _" | grep -v "PS.=" 
mini output = (CLAUDE_CODE_SSE_PORT=50062 COLORTERM=truecolor COMMAND_MODE=unix2003 DISPLAY=/private/tmp/com.apple.launchd.NtxTLe3G5d/org.xquartz:0 ENABLE_IDE_INTEGRATION=true GIT_ASKPASS=/private/var/folders/67/426psbzx2ng37qlgtftt2pq40000gn/T/AppTranslocation/916872CE-DCDB-449A-BFD8-7200145EEC68/d/Visual Studio Code.app/Contents/Resources/app/extensions/git/dist/askpass.sh HOME=/Users/mansoor_almarzooqi HOMEBREW_CACHE=/tmp/mansoor_almarzooqi/Homebrew/Caches HOMEBREW_CELLAR=/opt/homebrew/Cellar HOMEBREW_PREFIX=/opt/homebrew HOMEBREW_REPOSITORY=/opt/homebrew HOMEBREW_TEMP=/tmp/mansoor_almarzooqi/Homebrew/Temp INFOPATH=/opt/homebrew/share/info:/usr/local/share/info:/opt/homebrew/share/info:/usr/local/share/info: LANG=C.UTF-8 LESS=-R LOGNAME=mansoor_almarzooqi LSCOLORS=Gxfxcxdxbxegedabagacad LS_COLORS=di=1;36:ln=35:so=32:pi=33:ex=31:bd=34;46:cd=34;43:su=30;41:sg=30;46:tw=30;42:ow=30;43 MallocNanoZone=0 ORIGINAL_XDG_CURRENT_DESKTOP=undefined OSLogRateLimit=64 PAGER=less PATH=/Users/mansoor_almarzooqi/.antigravity/antigravity/bin:/Users/mansoor_almarzooqi/.brew/bin:/Users/mansoor_almarzooqi/goinfre/homebrew/bin:/usr/local/bin:/System/Cryptexes/App/usr/bin:/usr/bin:/bin:/usr/sbin:/sbin:/var/run/com.apple.security.cryptexd/codex.system/bootstrap/usr/local/bin:/var/run/com.apple.security.cryptexd/codex.system/bootstrap/usr/bin:/var/run/com.apple.security.cryptexd/codex.system/bootstrap/usr/appleinternal/bin:/opt/X11/bin:/Library/Apple/usr/bin:/usr/local/share/dotnet:/Users/mansoor_almarzooqi/.dotnet/tools:/Users/mansoor_almarzooqi/.local/bin:/Users/mansoor_almarzooqi/.local/bin:/Users/mansoor_almarzooqi/42-ValgrindContainer PWD=/Users/mansoor_almarzooqi/Desktop/MY_minishell_project_GitHub/minishell/minishell_tester SHELL=/bin/zsh SSH_AUTH_SOCK=/private/tmp/com.apple.launchd.VvQpIfw6c9/Listeners TERM=xterm-256color TERM_PROGRAM=vscode TERM_PROGRAM_VERSION=1.96.2 TMPDIR=/var/folders/67/426psbzx2ng37qlgtftt2pq40000gn/T/ USER=mansoor_almarzooqi USER_ZDOTDIR=/Users/mansoor_almarzooqi VSCODE_GIT_ASKPASS_EXTRA_ARGS= VSCODE_GIT_ASKPASS_MAIN=/private/var/folders/67/426psbzx2ng37qlgtftt2pq40000gn/T/AppTranslocation/916872CE-DCDB-449A-BFD8-7200145EEC68/d/Visual Studio Code.app/Contents/Resources/app/extensions/git/dist/askpass-main.js VSCODE_GIT_ASKPASS_NODE=/private/var/folders/67/426psbzx2ng37qlgtftt2pq40000gn/T/AppTranslocation/916872CE-DCDB-449A-BFD8-7200145EEC68/d/Visual Studio Code.app/Contents/Frameworks/Code Helper (Plugin).app/Contents/MacOS/Code Helper (Plugin) VSCODE_GIT_IPC_HANDLE=/var/folders/67/426psbzx2ng37qlgtftt2pq40000gn/T/vscode-git-4dfaea11d1.sock VSCODE_INJECTION=1 XPC_FLAGS=0x0 XPC_SERVICE_NAME=0 ZDOTDIR=/Users/mansoor_almarzooqi ZSH=/Users/mansoor_almarzooqi/.oh-my-zsh _=../minishell __CFBundleIdentifier=com.microsoft.VSCode __CF_USER_TEXT_ENCODING=0x1F5:0x0:0x0)
bash output = (declare -x CLAUDE_CODE_SSE_PORT="50062" declare -x COLORTERM="truecolor" declare -x COMMAND_MODE="unix2003" declare -x DISPLAY="/private/tmp/com.apple.launchd.NtxTLe3G5d/org.xquartz:0" declare -x ENABLE_IDE_INTEGRATION="true" declare -x GIT_ASKPASS="/private/var/folders/67/426psbzx2ng37qlgtftt2pq40000gn/T/AppTranslocation/916872CE-DCDB-449A-BFD8-7200145EEC68/d/Visual Studio Code.app/Contents/Resources/app/extensions/git/dist/askpass.sh" declare -x HOME="/Users/mansoor_almarzooqi" declare -x HOMEBREW_CACHE="/tmp/mansoor_almarzooqi/Homebrew/Caches" declare -x HOMEBREW_CELLAR="/opt/homebrew/Cellar" declare -x HOMEBREW_PREFIX="/opt/homebrew" declare -x HOMEBREW_REPOSITORY="/opt/homebrew" declare -x HOMEBREW_TEMP="/tmp/mansoor_almarzooqi/Homebrew/Temp" declare -x INFOPATH="/opt/homebrew/share/info:/usr/local/share/info:/opt/homebrew/share/info:/usr/local/share/info:" declare -x LANG="C.UTF-8" declare -x LESS="-R" declare -x LOGNAME="mansoor_almarzooqi" declare -x LSCOLORS="Gxfxcxdxbxegedabagacad" declare -x LS_COLORS="di=1;36:ln=35:so=32:pi=33:ex=31:bd=34;46:cd=34;43:su=30;41:sg=30;46:tw=30;42:ow=30;43" declare -x MallocNanoZone="0" declare -x OLDPWD declare -x ORIGINAL_XDG_CURRENT_DESKTOP="undefined" declare -x OSLogRateLimit="64" declare -x PAGER="less" declare -x PATH="/Users/mansoor_almarzooqi/.antigravity/antigravity/bin:/Users/mansoor_almarzooqi/.brew/bin:/Users/mansoor_almarzooqi/goinfre/homebrew/bin:/usr/local/bin:/System/Cryptexes/App/usr/bin:/usr/bin:/bin:/usr/sbin:/sbin:/var/run/com.apple.security.cryptexd/codex.system/bootstrap/usr/local/bin:/var/run/com.apple.security.cryptexd/codex.system/bootstrap/usr/bin:/var/run/com.apple.security.cryptexd/codex.system/bootstrap/usr/appleinternal/bin:/opt/X11/bin:/Library/Apple/usr/bin:/usr/local/share/dotnet:/Users/mansoor_almarzooqi/.dotnet/tools:/Users/mansoor_almarzooqi/.local/bin:/Users/mansoor_almarzooqi/.local/bin:/Users/mansoor_almarzooqi/42-ValgrindContainer" declare -x PWD="/Users/mansoor_almarzooqi/Desktop/MY_minishell_project_GitHub/minishell/minishell_tester" declare -x SHELL="/bin/zsh" declare -x SSH_AUTH_SOCK="/private/tmp/com.apple.launchd.VvQpIfw6c9/Listeners" declare -x TERM="xterm-256color" declare -x TERM_PROGRAM="vscode" declare -x TERM_PROGRAM_VERSION="1.96.2" declare -x TMPDIR="/var/folders/67/426psbzx2ng37qlgtftt2pq40000gn/T/" declare -x USER="mansoor_almarzooqi" declare -x USER_ZDOTDIR="/Users/mansoor_almarzooqi" declare -x VSCODE_GIT_ASKPASS_EXTRA_ARGS="" declare -x VSCODE_GIT_ASKPASS_MAIN="/private/var/folders/67/426psbzx2ng37qlgtftt2pq40000gn/T/AppTranslocation/916872CE-DCDB-449A-BFD8-7200145EEC68/d/Visual Studio Code.app/Contents/Resources/app/extensions/git/dist/askpass-main.js" declare -x VSCODE_GIT_ASKPASS_NODE="/private/var/folders/67/426psbzx2ng37qlgtftt2pq40000gn/T/AppTranslocation/916872CE-DCDB-449A-BFD8-7200145EEC68/d/Visual Studio Code.app/Contents/Frameworks/Code Helper (Plugin).app/Contents/MacOS/Code Helper (Plugin)" declare -x VSCODE_GIT_IPC_HANDLE="/var/folders/67/426psbzx2ng37qlgtftt2pq40000gn/T/vscode-git-4dfaea11d1.sock" declare -x VSCODE_INJECTION="1" declare -x XPC_FLAGS="0x0" declare -x XPC_SERVICE_NAME="0" declare -x ZDOTDIR="/Users/mansoor_almarzooqi" declare -x ZSH="/Users/mansoor_almarzooqi/.oh-my-zsh")
Test   7: ✅⚠️  exit 9223372036854775807 
mini error = ( numeric argument required)
bash error = ()
Test   8: ✅ exit 9223372036854775808 
Test   9: ❌ exit -9223372036854775807 
mini exit code = 255
bash exit code = 1
mini error = ( numeric argument required)
bash error = ()
Test  10: ❌ exit -9223372036854775808 
mini exit code = 255
bash exit code = 0
mini error = ( numeric argument required)
bash error = ()
Test  11: ✅ exit -9223372036854775809 
4/11
😭 😭 😭
➜  minishell_tester git:(main) ✗ 