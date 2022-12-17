# procDump

Simple Program to dump any process using the process ID, as it will search in running process on the system to find the targeted process and once found, it will dump it and save the dump with the process excutable name with adding `.dmp` as the dump extention.

This program uses `MiniDumpWriteDump` windows function to dump the process, so even if the `Task Manager` is disabled, you still can dump the memory of any process.

This tool is created to evade using any known malicious tool such as `Mimikatz` or even the known `ProcDumper` tool, as if it was identified by its hash to be prvented from being used to dump the process memory, so you can still dump the memeory of any targeted running process.

The main purpose of this tool is to dump `LSASS` process to be pared offline using Mimikatz.

## Usage
Open `cmd` or `powershell` as ADMINSITRATOR if you are going to dump `LSASS.exe`.
Run the executable
```
.\procDump.exe <PID>
```
![image](https://user-images.githubusercontent.com/14153248/208212785-3073af4e-e757-49fa-9e67-cb8e496d1d41.png)
 and the created DUMP file will be in the same directory of the tool.
 ![image](https://user-images.githubusercontent.com/14153248/208212898-6ffdc463-d61c-461e-8cda-e2bf48d73e07.png)


## Testin Results
This tool has been tested on Windows 10 and Windows 11.
