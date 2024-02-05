import { Component, OnInit, ViewChild, ViewContainerRef, OnDestroy, ComponentFactoryResolver } from '@angular/core';
import { NetService } from './net.service';
import { NetTree } from './classes/net-tree';
import { netComponents } from './classes/netComponents';
import { TiLeftmenuItem } from '@cloud/tiny3';
import { LoadingService } from 'src/app/common-app/service';

@Component({
    selector: 'app-net',
    templateUrl: './net.component.html',
    styleUrls: ['./net.component.scss']
})
export class NetComponent implements OnInit, OnDestroy {

    @ViewChild('cardContainer', { static: false, read: ViewContainerRef }) componentContainer: ViewContainerRef;
    public id: string = 'netLeftMenu';
    // 默认展开，当设置为true时会收起
    public collapse: boolean = false;
    public toggle: boolean = false;
    // 初始设置为true
    public reload: boolean = true;
    public routable: boolean = false;
    public active: TiLeftmenuItem;
    public netTree: NetTree;
    private cmpRef: any;

    public leftMenuHightLight: number = 0;
    constructor(
        private netService: NetService,
        private componentFactoryResolver: ComponentFactoryResolver,
        private loading: LoadingService
    ) {
        this.netTree = new NetTree();
    }

    public displayComponent(index: number, num: number): void {
        try {
            this.netService.activeNum = num;
            this.netService.activeIndex = index;
            this.leftMenuHightLight = num;
            if (this.componentContainer) {
                this.componentContainer.clear();
            }
            if (this.netTree.getCardArray.length > 0) {
                const component = this.componentFactoryResolver.resolveComponentFactory(
                    netComponents[this.netTree.getCardArray[index].type]
                );
                this.cmpRef = this.componentContainer.createComponent(component);
            }
        } catch (error) {
            this.loading.state.next(false);
        }
    }

    ngOnInit() {
        this.loading.state.next(true);
        this.netService.factory().subscribe(
            (result: NetTree) => {
                this.netTree = result;
                this.netService.netTree = this.netTree;
                this.displayComponent(0, 0);
                if (this.netTree.getCardArray.length > 0) {
                    this.active = this.netTree.getCardArray[0].cards[0];
                }
                this.loading.state.next(false);
            },
            () => {
                this.loading.state.next(false);
            }
        );
    }

    ngOnDestroy() {
        if (this.cmpRef) {
            this.cmpRef.destroy();
        }
    }
}
