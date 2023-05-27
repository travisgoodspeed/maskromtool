function Component(){
    //Install to C:\maskromtool\ on Windows, /opt/maskromtool on unix.
    if (installer.value("os") === "win") {
        installer.setValue("TargetDir", "c:\maskromtool");
    }else{
        installer.setValue("TargetDir", "/opt/maskromtool");
    }

    
}

Component.prototype.createOperations=function(){
    //Default operation installs the files.
    component.createOperations();
    

    //Windows entries in the Start menu.
    if (installer.value("os") === "win") {
        component.addOperation("CreateShortcut", "@TargetDir@/maskromtool.exe",
                               "@StartMenuDir@/maskromtool.lnk");
    }
}
