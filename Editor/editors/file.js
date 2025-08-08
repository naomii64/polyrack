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
        table.newRow();

        table.addText("module ID:",UIHelper.ALIGN_RIGHT);
        const moduleIDDisplay = UIHelper.quickDiv();
        moduleIDDisplay.changeID=()=>{
            //create an ID
            //IDS are [unix time of id generation]-[a random 8 digit number]
            const randomNumberLength=8;
            
            let randomNumber='';
            const timeOfCreation = Date.now().toString();
            
            for(let i=0;i<randomNumberLength;i++){
                randomNumber+=Math.round(Math.random()*9).toString();
            }
            
            project.ID = `${timeOfCreation}-${randomNumber}`;
            moduleIDDisplay.textContent=project.ID;
        };
        moduleIDDisplay.changeID();
        const moduleIDchangeButton=UIHelper.quickButton();
        moduleIDchangeButton.innerHTML='&#10227;';
        moduleIDchangeButton.onclick=()=>{
            moduleIDDisplay.changeID();
        };
        table.addElm(moduleIDDisplay);
        table.addElm(moduleIDchangeButton);
    }
}