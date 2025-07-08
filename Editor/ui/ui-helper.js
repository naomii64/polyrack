class UIHelper{
    static ALIGN_LEFT='left';
    static ALIGN_CENTER='center';
    static ALIGN_RIGHT='right';
    static shiftKey=false;
    static elms=[];
    static init(){
        document.addEventListener('keydown',(e)=>{
            if(e.key=='Shift'){
                UIHelper.shiftKey=true;
            }
        });
        document.addEventListener('keyup',(e)=>{
            if(e.key=='Shift'){
                UIHelper.shiftKey=false;
            }
        });
    }
    static linkedIntInput(obj,key){
        const input=document.createElement('input');
        input.type='number';
        input.step='1';
        input.oninput=function(){
            obj[key]=parseInt(this.value);
        };
        input.updateVal=function(){
            this.value=obj[key];
        }
        input.updateVal();
        UIHelper.elms.push(input);
        return input;
    }
    static linkedTextInput(obj,key){
        const input=document.createElement('input');
        input.type='text';
        input.oninput=function(){
            obj[key]=this.value;
        };
        input.updateVal=function(){
            this.value=obj[key];
        }
        input.updateVal();
        UIHelper.elms.push(input);
        return input;
    }
    static linkedInput(obj,key,type='text'){
        const input=document.createElement('input');
        input.type=type;
        input.oninput=function(){
            obj[key]=this.value;
        };
        input.updateVal=function(){
            this.value=obj[key];
        }
        input.updateVal();
        UIHelper.elms.push(input);
        return input;
    }
    static textArrayInput(array){
        const tbl=UIHelper.quickTable();
        tbl.regenerate=function(){
            this.reset();
            for(let i=0;i<array.length;i++){
                //create elements
                const entryContainer=UIHelper.quickDiv();
                const removeButton=UIHelper.quickButton('X');
                const textInput=UIHelper.linkedTextInput(array,i);
                //style
                entryContainer.style.display='flex';
                textInput.style.minWidth=0;
                textInput.style.flexGrow=1;
                textInput.style.width='100%';
                //build
                removeButton.onclick=()=>{
                    array.splice(i,1);
                    tbl.regenerate();
                };
                entryContainer.appendChild(removeButton);
                entryContainer.appendChild(textInput);
                this.addElm(entryContainer,false);
            }
            //add the button that lets you extend the array
            const addButton=UIHelper.quickButton('+');
            this.addElm(addButton);
            addButton.onclick=()=>{
                array.push('');
                tbl.regenerate();
            };
        };
        tbl.regenerate();
        UIHelper.elms.push(tbl);
        tbl.updateVal=tbl.regenerate;
        return tbl;
    }
    static fixedLengthArrayInput(arr,type='text',sepChar='x'){
        const outerDiv=UIHelper.quickDiv();
        for(let i=0;i<arr.length;i++){
            const input=UIHelper.linkedInput(arr,i,type);
            outerDiv.appendChild(input);
            input.style.flexGrow=1;
            input.style.minWidth=0;
            input.style.width='100%';
            if(i>=arr.length-1){continue;}
            const seperator=UIHelper.quickDiv(sepChar);
            outerDiv.appendChild(seperator);
        }
        outerDiv.style.display='flex';
        outerDiv.updateVal=function(){};
        UIHelper.elms.push(outerDiv);
        return outerDiv;
    }
    static quickButton(text=""){
        const button=document.createElement('button');
        button.textContent=text;
        return button;
    }
    static quickDiv(text=''){
        const div=document.createElement('div');
        if(text.length>0){div.textContent=text;}
        return div;    
    }
    static encapsulateElm(outerElmType,elm){
        const th=document.createElement(outerElmType);
        th.appendChild(elm);
        return th;
    }
    static quickTable(){
        const table=document.createElement('table');
        table.newRow=function(){
            this.tr=document.createElement('tr');
            this.appendChild(this.tr);
            return this.tr;
        };
        table.newRow();
        table.addElm=(elm,stretchToFit=true)=>{
            const th=document.createElement('th');
            table.tr.appendChild(th);
            th.appendChild(elm);
            if(stretchToFit){
                elm.style.width='100%';
            }
            return th;
        }
        table.addText=(text,alignment=UIHelper.ALIGN_CENTER)=>{
            const th=document.createElement('th');
            if(table.tr==null){table.newRow();}
            table.tr.appendChild(th);
            th.textContent=text;
            th.style.textAlign=alignment;
            return th;
        }
        table.reset=function(){
            this.innerHTML='';
            this.newRow();
        };
        return table;
    }
    //update the value of every element to reflect change
    static updateAll(){
        for(const v of UIHelper.elms){
            if(v.updateVal==null){continue;}
            v.updateVal();
        }
    }
}