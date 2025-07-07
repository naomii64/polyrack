class FileMenu{
    static init(){
        //get elements
        FileMenu.moduleSettingsDiv=document.getElementById('module-settings-div');
        //create UI
        const table=UIHelper.quickTable();
        FileMenu.moduleSettingsDiv.appendChild(table);
        table.addText("module name:",UIHelper.ALIGN_RIGHT);
        table.addElm(UIHelper.linkedTextInput(project,'name'));
        table.newRow();
        table.addText("module desciption:",UIHelper.ALIGN_RIGHT);
        table.addElm(UIHelper.linkedTextInput(project,'description'));
        table.newRow();
        table.addText("module tags:",UIHelper.ALIGN_RIGHT);
        table.addElm(UIHelper.textArrayInput(project.tags));
    }
}