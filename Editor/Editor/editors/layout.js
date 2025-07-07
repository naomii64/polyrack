class LayoutEditor{
    static components=[];

    static init(){
        //get elements
        LayoutEditor.layout=document.getElementById("layout-div");
        LayoutEditor.list=document.getElementById("layout-component-list");
        LayoutEditor.addButton=document.getElementById("layout-add-button");
        LayoutEditor.addList=document.getElementById("layout-add-list");
        LayoutEditor.compAttribs=document.getElementById("component-attributes");
        LayoutEditor.extrasCheckbox=document.getElementById("extras-checkbox");

        //setup renderer
        LE.renderer=GLOBAL.createView();

        LE.layout.appendChild(LE.renderer.canvas);

        project.layout.width=4;
        project.layout.height=4;
        LE.updateGrid();

        project.layout.components=[];

        LE.sizeTableRow=document.getElementById("layout-size-tr");
        const widthInput=UIHelper.linkedIntInput(project.layout,'width');
        const heightInput=UIHelper.linkedIntInput(project.layout,'height');
        LE.sizeTableRow.appendChild(UIHelper.encapsulateElm('th',widthInput));
        LE.sizeTableRow.appendChild(UIHelper.encapsulateElm('th',heightInput));
        widthInput.addEventListener('input',()=>{LE.updateGrid();});
        heightInput.addEventListener('input',()=>{LE.updateGrid();});

        LE.registerComponentTypes()

        LE.addList.style.height=0;
        LE.addButton.open=false;
        LE.generateAddComponentMenu();

        LE.addButton.addEventListener('click',e=>{
            LE.addButton.open=!LE.addButton.open;
            if(LE.addButton.open){
                LE.addButton.textContent='(close menu)';

                const addMenuHeight=LE.addButtonHeight*LE.components.length;
                LE.addList.style.height=`${addMenuHeight}px`;
            }else{
                LE.addButton.textContent='ADD';
                LE.addList.style.height=0;
            }
        })
    }
    static addButtonHeight=20;
    static addByType(type){
        const comp=LE.components.find(v=>v.type==type);
        return LE.addComponent(comp);
    }
    static addComponent(comp){
        const button=document.createElement('button');
        button.textContent=comp.type;
        button.classList.add('list-button');
        const newComp={};
        newComp.init=comp.init;
        newComp.onUpdate=comp.onUpdate;
        newComp.draw=comp.draw;
        newComp.parent=comp;
        newComp.button=button;
        newComp.init();
        newComp.onUpdate();
        project.layout.components.push(newComp)
        LE.list.appendChild(button);
        button.addEventListener('click',()=>{
            LE.compAttribs.innerHTML='';
            comp.loadUI(newComp);
            newComp.onUpdate();
            LE.renderer.axis.position=newComp.position;
        })
        return newComp;
    }
    static generateAddComponentMenu(){
        for(let v of LE.components){
            const button=document.createElement('button');
            button.style.height=`${LE.addButtonHeight}px`
            LE.addList.appendChild(button)
            button.textContent=`add ${v.type}`
            button.classList.add('add-component-button');
            button.addEventListener('click',()=>{
                LE.addComponent(v);
            })
        }
    }
    //for every type of component
    static universalInit(comp){
        for(const v of comp.parent.savableKeys){
            comp[v.key]=v.defaultValue;
        }
    }
    static universalUpdate(comp){
        if(!comp.name||comp.name.length==0){comp.name="component"}
        if(!comp.position){comp.position=[0,0,0]}
        comp.button.innerHTML=comp.name;
        const hint=document.createElement('div');
        hint.textContent=` (${comp.parent.type})`;
        comp.button.appendChild(hint);
        hint.classList.add('hint');
    }
    static universalLoadUI(comp){
        LE.compAttribs.appendChild(LE.createLabel('Name:'));
        LE.compAttribs.appendChild(LE.linkedInput(comp,'name'));

        LE.compAttribs.appendChild(LE.createLabel('position:'));
        LE.compAttribs.appendChild(LE.vectorInput(comp.position));
        LE.compAttribs.appendChild(LE.createLabel('rotation:'));
        LE.compAttribs.appendChild(LE.vectorInput(comp.rotation));
        LE.compAttribs.appendChild(LE.createLabel('scale:'));
        LE.compAttribs.appendChild(LE.vectorInput(comp.scale));
        
        LE.compAttribs.appendChild(LE.rule());
            
    }

    //register them
    static registerComponentTypes(){
        LE.components=[];
        const registerComponent=function(name,init=function(){}){
            const comp={};
            comp.type=name;
            LE.components.push(comp);
            //functions that can be overridden
            comp.onUpdate=function(){};
            comp.loadUI=function(){}
            comp.draw=function(){}
            comp.savableKeys=[];
            comp.addSavableKey=function(keyName,defaultValue){
                const savableKey={};
                savableKey.key=keyName;
                savableKey.defaultValue=defaultValue;
                this.savableKeys.push(savableKey);
            }
            comp.addSavableKey('name','[component]');
            comp.addSavableKey('position',[0,0,0]);
            comp.addSavableKey('rotation',[0,0,0]);
            comp.addSavableKey('scale',[1,1,1]);
            comp.init=init;
            return comp;
        }
        //register the components
        const mesh=registerComponent('static mesh',function(){
            LE.universalInit(this)
            this.noSave={};
        });
        mesh.addSavableKey("modelID",0);

        mesh.loadUI=function(comp){
            LE.universalLoadUI(comp);
            
            LE.compAttribs.appendChild(LE.createLabel('model ID:'));
            LE.compAttribs.appendChild(LE.modelIDInput(comp,'modelID'));
        }
        mesh.onUpdate=function(){
            LE.universalUpdate(this);
            if(project.models.length>0){
                this.noSave.mesh=LE.renderer.newMeshRaw(project.models[this.modelID].data);
                this.noSave.mesh.texture=MV.getTexture(project.models[this.modelID].textureID,LE.renderer);
            }else{
                this.noSave.mesh=LE.renderer.newMeshRaw(Meshes.cube());
            }
        }
        mesh.draw=function(renderer){
            this.noSave.mesh.position=this.position;
            this.noSave.mesh.rotation=Vec3.mulf(this.rotation,Math.PI/180);
            this.noSave.mesh.scale=this.scale;
            
            renderer.drawMesh(this.noSave.mesh);
        }
        //for inputs
        const input=registerComponent('input',function(){
            LE.universalInit(this)
            this.animation=null;
        });
        input.addSavableKey("animationID",0);

        input.onUpdate=function(){
            LE.universalUpdate(this);
            this.models=[];
            if(project.animations.length>0){
                this.animation=AnimationModel.from(project.animations[this.animationID],LE.renderer);
            }
            if(this.animation){
                this.animation.position=this.position;
            }
        }
        input.draw=function(renderer){
            if(this.animation){
                this.animation.draw(renderer);
            }
        }
        input.loadUI=function(comp){
            LE.universalLoadUI(comp)
            
            LE.compAttribs.appendChild(LE.createLabel('animation ID:'));
            LE.compAttribs.appendChild(LE.modelIDInput(comp,'animationID'));
        }
        //sockets
        const socket=registerComponent('socket',function(){
            LE.universalInit(this);
            this.mesh=LE.renderer.newMeshRaw(EditorAssets.CABLE_SOCKET_DATA);
            this.mesh.texture=LE.renderer.createGLTexture(EditorAssets.MODULE_TEXTURE_DATA);
        });
        socket.addSavableKey('isInput',true);
        socket.loadUI=function(comp){
            LE.universalLoadUI(comp);
            LE.compAttribs.appendChild(LE.toggleInput(comp,'isInput','input type: IN','input type: OUT'));
        }
        socket.onUpdate=function(){
            LE.universalUpdate(this);
            this.mesh.position=this.position;
        }
        socket.draw=function(renderer){
            renderer.drawMesh(this.mesh);
        }
    }
    static rule(){return document.createElement('hr')}
    static updateGrid(){
        LE.grid=LE.renderer.newMeshRaw(Meshes.grid(project.layout.width,project.layout.height));
        LE.renderer.setDragCenter(project.layout.width/2,project.layout.height/2,0);
    }
    static animate(){
        LE.renderer.stretchToParent();
        if(LE.extrasCheckbox.checked){
            LE.renderer.drawAxis();
            LE.renderer.drawMesh(LE.grid);
        }
        for(let v of project.layout.components){
            v.draw(LE.renderer);
        }
    }
    static toggleInput(object,key,displayTrue='true',displayFalse='true'){
        const button=document.createElement('button');
        button.onclick=function(){
            object[key]=!object[key];
            this.updateText();
        }
        button.updateText=function(){
            this.textContent=(object[key]?displayTrue:displayFalse)
        }
        button.updateText();
        return button;
    }
    //quick functions 
    static createLabel(text){
        const div=document.createElement('div');
        div.textContent=text;
        div.classList.add('label')
        return div;
    }
    static modelIDInput(object,key){
        const input=document.createElement('input');
        input.type='number';
        input.step=1;
        input.min=0;
        input.max=project.models.length-1;
        input.value=object[key];
        input.onchange=function(){object[key]=parseInt(this.value);object.onUpdate()}
        return input;
    }
    static linkedInput(obj,key){
        const input=document.createElement('input');
        input.value=obj[key];
        input.onchange=function(){obj[key]=this.value;obj.onUpdate()}

        return input;
    }
    static vectorInput(arr,object){
        const addInput=function(arrayIndex){
            const outerDiv=document.createElement('div');
            outerDiv.style.flexGrow=1;
            div.appendChild(outerDiv);
            const input=document.createElement('input');
            outerDiv.appendChild(input);
            input.style.width='100%';
            input.type='number';
            input.value=arr[arrayIndex];
            input.oninput=function(e){
                const value=parseFloat(this.value);
                //typing in a decimal point resulted in the value resetting
                if(arr[arrayIndex]==value){return;}
                
                const delta=Math.sign(value-arr[arrayIndex]);
                if(!UIHelper.shiftKey){
                    arr[arrayIndex]=value;
                }else{
                    const slowness=0.1;
                    arr[arrayIndex]+=delta*slowness;    
                    arr[arrayIndex]=Math.round(arr[arrayIndex]*10)/10;
                }
                this.value=arr[arrayIndex];
            }
        }
        const addX=function(){
            const x=document.createElement('div');
            x.textContent='X';
            div.appendChild(x);
        }

        const div=document.createElement('div');
        div.style.display='flex';

        addInput(0);
        addX();
        addInput(1);
        addX();
        addInput(2);

        return div;
    
    }
}
const LE=LayoutEditor;

class EditorAssets{
    static CABLE_SOCKET=`
v 0.190061 0.000000 0.100000
v 0.095030 0.162909 0.100000
v -0.095030 0.162909 0.100000
v -0.190061 0.000000 0.100000
v -0.095030 -0.162909 0.100000
v 0.095030 -0.162909 0.100000
v 0.380861 0.000000 0.100000
v 0.190430 0.330748 0.100000
v 0.190430 0.330747 -0.100000
v 0.380861 0.000000 -0.100000
v -0.190430 0.330748 0.100000
v -0.190430 0.330747 -0.100000
v -0.380861 0.000000 0.100000
v -0.380861 0.000000 -0.100000
v -0.190430 -0.330747 0.100000
v -0.190430 -0.330747 -0.100000
v 0.190430 -0.330747 0.100000
v 0.190430 -0.330747 -0.100000
v 0.133615 0.236397 0.200000
v 0.267231 0.000000 0.200000
v -0.133615 -0.236396 0.200000
v -0.267231 0.000000 0.200000
v -0.133615 0.236397 0.200000
v 0.133615 -0.236396 0.200000
vn -0.3358 0.5824 0.7403
vn 0.3388 0.5790 0.7416
vn -0.3388 0.5790 0.7416
vn -0.6630 -0.0000 0.7486
vn -0.6606 -0.0000 0.7507
vn 0.6630 -0.0000 0.7486
vn 0.3358 0.5824 0.7403
vn -0.3388 -0.5790 0.7416
vn -0.3358 -0.5824 0.7403
vn -0.0000 -0.0000 1.0000
vn 1.0000 -0.0000 -0.0000
vn 0.5005 0.8657 -0.0000
vn -0.5005 0.8657 -0.0000
vn -1.0000 -0.0000 -0.0000
vn -0.5005 -0.8657 -0.0000
vn 0.5005 -0.8657 -0.0000
vn 0.3388 -0.5790 0.7416
vn 0.6606 -0.0000 0.7507
vn 0.3358 -0.5824 0.7403
vn 0.3966 0.6625 0.6355
vn -0.3832 0.6675 0.6384
vn -0.3966 0.6625 0.6355
vn -0.7873 -0.0000 0.6166
vn -0.7917 -0.0000 0.6109
vn 0.7873 -0.0000 0.6166
vn 0.3832 0.6675 0.6384
vn -0.3966 -0.6625 0.6355
vn -0.3832 -0.6675 0.6384
vn 0.3966 -0.6625 0.6355
vn 0.7917 -0.0000 0.6109
vn 0.3832 -0.6675 0.6384
vt 0.675000 0.916667
vt 0.525000 0.805556
vt 0.675000 0.805556
vt 0.400000 0.916667
vt 0.400000 0.805556
vt 0.800000 0.805556
vt 0.800000 0.916667
vt 0.650000 0.638889
vt 0.525000 0.444444
vt 0.400000 0.638889
vt 0.400000 0.694444
vt 0.650000 0.694444
vt 0.650000 0.805556
vt 0.525000 0.694444
vt 0.525000 0.916667
vt 0.400000 1.000000
vt 0.525000 1.000000
vt 0.675000 1.000000
vt 0.800000 1.000000
vt 0.650000 0.500000
vt 0.400000 0.500000
vt 0.800000 0.694444
f 23/1/1 8/2/2 11/3/3
f 22/4/4 11/2/3 13/5/5
f 20/1/6 8/6/2 19/7/7
f 22/7/4 15/3/8 21/1/9
f 2/8/10 4/9/10 6/10/10
f 10/11/11 8/2/12 7/5/11
f 9/12/12 11/6/13 8/13/12
f 12/11/13 13/2/14 11/5/13
f 13/2/14 16/12/15 15/13/15
f 16/12/15 17/6/16 15/13/15
f 18/14/16 7/13/11 17/2/16
f 20/1/6 17/2/17 7/3/18
f 21/4/9 17/2/17 24/15/19
f 5/16/20 24/15/21 6/17/22
f 6/17/22 20/1/23 1/18/24
f 5/19/20 22/1/25 21/7/26
f 2/19/27 20/1/23 19/7/28
f 3/16/29 22/15/25 4/17/30
f 2/17/27 23/1/31 3/18/29
f 23/1/1 19/15/7 8/2/2
f 22/4/4 23/15/1 11/2/3
f 20/1/6 7/3/18 8/6/2
f 22/7/4 13/6/5 15/3/8
f 6/10/10 1/14/10 2/8/10
f 2/8/10 3/20/10 4/9/10
f 4/9/10 5/21/10 6/10/10
f 10/11/11 9/14/12 8/2/12
f 9/12/12 12/22/13 11/6/13
f 12/11/13 14/14/14 13/2/14
f 13/2/14 14/14/14 16/12/15
f 16/12/15 18/22/16 17/6/16
f 18/14/16 10/12/11 7/13/11
f 20/1/6 24/15/19 17/2/17
f 21/4/9 15/5/8 17/2/17
f 5/16/20 21/4/26 24/15/21
f 6/17/22 24/15/21 20/1/23
f 5/19/20 4/18/30 22/1/25
f 2/19/27 1/18/24 20/1/23
f 3/16/29 23/4/31 22/15/25
f 2/17/27 19/15/28 23/1/31 `;
    static CABLE_SOCKET_DATA=parseOBJ(EditorAssets.CABLE_SOCKET);
    static MODULE_TEXTURE='data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACgAAAAkCAYAAAD7PHgWAAAAAXNSR0IArs4c6QAABDZJREFUWEfNl0tIVFEYx89QpIGY6UhPLKRSEMJs0UJXIhFFRBCIqwgKimpTRBRF0KKQaBVRYLgTl9GmhQhuctsLhKwYSnqJo6gIaRQT/4P/23e/+c7MnSmhs7kz53V/5/+9zk3dufc451Rr3tUY9TQ1bdLDbnz8q+978zbjJiYyeeOldIw+7XNVq1e5hZ+//LLZuXn/nF9YctVVFS51/tLdCLChodG/EE8LUoNxPoHwP9S4rzwQ5g8+vOKXWJA166r/AEo4voSQVFECYo5+GSEkpOyz5lNBrIGKUBDqoTVsqc9XUMLBhBKSgDQvnkcOt7ud2+pN4d59nPJjeFoNY/Xr0x7EH/hz/rwUfFCfLKmCmIcD4NQwB/3HgsG41W5c6PbmpYL0Q6hoKljMB6Eqzat9lZuHYNkvYQnIIOEeeMZ8UDo6VEliXhxmf1dXnjCWkhYcFmoF6YOIYLSYiWUUSkgrSKTS2PTbZDYC3bghHYPGGPr0HECf6O4s7IM6zcidk0RxodRiOp3qRJqRPlgwD/rQXs5lGo77IpLph/RFrpO5zkpb1rhOM3k+2LKjMa+SVFSsic7J08mD06GXln64zqNn84TSCVz6t648ViVhFfFB0rh1cwS4trLSfV9cdHhakBKM8yazc74khRqTbnZ2OmXNSdfU5QrmQQJKOG5ESJ2noBwaINHkiSWEjMjMpy8mYFtLc07vz4Tta7FWUMIBRELKXEVIrSBznE7ehQDxTtZimaK8ieGDVAIToWRSBaWKVqWQkMUU1Ik6qiSl+qA0L90iVMZ4UICGABP7IDdjgCQxr1Qe67V5ZcoIARbLlTETa/MS0goSHcW6lCX1waKAoSDBwiRRzDTCVCPNLatCKM2UBCiDRMNxIzgz/RB9jGKmGv1C9pdt4mP79sYqyfGb/f4d8ltEXlTRL/+j2CdtUBFBUcr8FBKlXHDu6gNXn671gACZys646ZnZ2PV+T2ubX/Li5XM32HcrWs6KIPeTt+QQ4O70dvc6+yGPG/NNQMzsaG9xz0bH/CINyAsF6ioBJRyg9H/sowE1mP5fEiBewNtIXW1NTEFZS0OggF4xE1NFQtLEULb3+smYWiH/0oAhs8r+mILMW+0HT/l3hK5MGgAmhmrarJzH/n+qIDZHoCBA2KAWGs1LVbWCuthL0LJNLLP+td4Bv+d/A4hKIstUz+nbkVJUTZuVKtIHvUssf3xbPshUYykIn2PTqcb7IEsdIc9cvu/n45tkaHjY/0ZQSBPzt0wzhNQmTpIHQ4EVA+SkQz0XQ/PNwJGJOrhweaAsH5SXBexjKaijWibqV0P97tGTEXegozXik7cido69z5hX/mKH8h/u0qe4AH0oZRac/DLTgNa3DfYoG7B/YCQHn9KQ+vMwlBclYAjurwGZ06TcFiDGdb+loGW2shWkiWVkFvMLOb7SPvgbYmXQptzsTjwAAAAASUVORK5CYII=';
    static MODULE_TEXTURE_DATA;
}
base64ToTextureData(EditorAssets.MODULE_TEXTURE,function(texture){
    EditorAssets.MODULE_TEXTURE_DATA=texture;
});