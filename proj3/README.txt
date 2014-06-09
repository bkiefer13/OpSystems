Daniel Razavi 4134953
Brynn Kiefer  4785044

Proj 3

For this project, we started off with just trying to get the immediate files created. We believe this works, and we tested creating immediate files using "echo akdhfsdlkf > some_file", which works. But, when trying to convert the immediate file into a regular file, we ran into some issues. We got this error "fs_putnode failed" when we expanded the file to be more than 32 bytes. When searching through the code, we believe this error is triggered when the inode is incorrectly allocated. But, we were unable to figure out how to fix this bug. So, we moved on to trying to implement LSR, since we were running out of time, and wanted to make sure we had something for the second part so that we could get some credit for at least getting this far. We didn't have much time to test LSR, so we are unsure if it works, but the idea is there. 
