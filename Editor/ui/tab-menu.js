class TabMenus{
    //where menus are stored for later manipulation
    static menus={};
    static init(){
        //tab menu script
        const tabMenus=document.getElementsByClassName('tab-menu');
        for(const v of tabMenus){
            const currentMenuObj={};
            currentMenuObj.selectedTab=null;
            currentMenuObj.tabs={};
            TabMenus.menus[v.id]=currentMenuObj;
            const bar=document.createElement('div');
            bar.classList.add('tab-bar');
            bar.classList.add('shade');
            const container=document.createElement('div');
            container.classList.add('tab-container');
            container.classList.add('shade');

            const tabs=[];
            //do all the tabs
            for(const b of v.children){
                if(b.localName=="tab"){
                    const tabObj={};
                    tabObj.name="";
                    tabObj.animate=null;
                    if(b.id!=""){
                        currentMenuObj.tabs[b.id]=tabObj;
                    }
                    const tab={name:"",content:"",id:b.id,menu:currentMenuObj};
                    tabs.push(tab);

                    for(const n of b.children){
                        if(n.localName=="tabtitle"){
                            tab.name=n.innerHTML;
                            tabObj.name=n.innerHTML;
                        }
                        if(n.localName=="tabcontent"){
                            tab.content=n.innerHTML;
                        }
                    }
                }
            }
            while(v.children.length>0){v.children[0].remove()}
            //now create the tabs
            for(const b of tabs){
                const button=document.createElement('button');
                button.textContent=b.name;
                bar.appendChild(button);
                const contentContainer=document.createElement('div');
                contentContainer.classList.add('tab-contents')
                contentContainer.innerHTML=b.content;
                contentContainer.style.size="100%";
                container.appendChild(contentContainer);
                button.container=contentContainer;
                button.tabMenuContainer=container;
                button.classList.add('tab-button');
                button.tabID=b.id;
                button.menu=b.menu;
                button.onclick = function(){
                    for(const v of this.tabMenuContainer.children){
                        v.style.display='none';
                    }
                    this.container.style.display=null;
                    highlightButtonInList(this);
                    this.menu.selectedTab=this.tabID;
                }
            }
            setTimeout(function(){bar.children[0].onclick()},0);
            v.appendChild(bar);
            v.appendChild(container);
        }
        //start animating tabs
        const animate=()=>{
            TabMenus.animateTabs();
            requestAnimationFrame(animate);
        }
        requestAnimationFrame(animate);
    }
    static animateTabs(){
        for(const k of Object.keys(TabMenus.menus)){
            const menu=TabMenus.menus[k];
            if(menu.selectedTab==null){return;}
            if(menu.selectedTab==""){return;}
            if(menu.tabs[menu.selectedTab]==null){return;}
            if(menu.tabs[menu.selectedTab].animate==null){return;}
            menu.tabs[menu.selectedTab].animate();
        }
    }
    static setTabDrawFunction(menuID,tabID,func){
        const menu=TabMenus.menus[menuID];
        if(menu==null){return;}
        const tab=menu.tabs[tabID];
        if(tab==null){return;}
        tab.animate=func;
    }
}