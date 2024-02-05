import { Component, AfterViewInit, ViewChild, ViewContainerRef, ComponentFactoryResolver, OnDestroy } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { OthersService } from './others.service';
import { dynamicComponent } from './dynamicComponent';
import { NavTree } from './models';
import { LoadingService } from 'src/app/common-app/service';

@Component({
    selector: 'app-others',
    templateUrl: './others.component.html',
    styleUrls: ['./others.component.scss'],
    providers: [
        OthersService,
    ]
})
export class OthersComponent implements AfterViewInit, OnDestroy {

    @ViewChild('cardContainer', { static: false, read: ViewContainerRef }) componentContainer: ViewContainerRef;

    public navTree: NavTree;
    public cmpRef: any;

    constructor(
        private otherService: OthersService,
        private componentFactoryResolver: ComponentFactoryResolver,
        public i18n: TranslateService,
        private loading: LoadingService
    ) {
        this.navTree = new NavTree();
    }

    displayComponent(index: number) {
        try {
            this.loading.state.next(true);
            if (this.componentContainer) {
                this.componentContainer.clear();
            }
            this.otherService.activeCard = this.navTree.getCardUrlArray[index];
            this.navTree.getCardUrlArray.forEach((val) => val.setActive = false);
            this.navTree.getCardUrlArray[index].setActive = true;
            const component = this.componentFactoryResolver.resolveComponentFactory(
                dynamicComponent[this.navTree.getCardUrlArray[index]?.getComponentName]
            );
            this.cmpRef = this.componentContainer.createComponent(component);
        } catch (error) {
            this.loading.state.next(false);
        }
    }

    ngAfterViewInit() {
        this.loading.state.next(true);
        this.otherService.init().subscribe(
            (val) => {
                this.navTree = this.otherService.navTree;
                if (this.navTree.getCardUrlArray[0]) {
                    this.navTree.getCardUrlArray[0].setActive = true;
                }
                this.displayComponent(0);
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
