function Component(){
  //Empty constructor
}

Component.prototype.createOperations=function(){
  //Default operation installs the files.
  component.createOperations();
  
  if(systemInfo.productType === "windows") {
    //installer.setValue("TargetDir", "c:\maskromtool");
    component.addOperation("CreateShortcut", "@TargetDir@/maskromtool.exe",
      "@StartMenuDir@/maskromtool.lnk");

    //This gets auto-removed for some reason.
    //component.addOperation("CreateShortcut", "@TargetDir@/maintenancetool.exe",
    //  "@StartMenuDir@/maintenancetool.lnk");
  }else{  //Linux and macOS.
    installer.setValue("TargetDir", "/opt/maskromtool");
  }
}
