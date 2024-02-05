import { Component, OnInit, OnDestroy, ViewChild, ViewContainerRef, ComponentFactoryResolver, AfterViewInit } from '@angular/core';

import { GlobalData, WebStyle } from 'src/app/common-app/models';
import { homeComponent } from './component/homeComponent';


@Component({
    selector: 'app-home',
    templateUrl: './home.component.html',
    styleUrls: ['./home.component.scss']
})
export class HomeComponent implements OnInit, AfterViewInit, OnDestroy {

    constructor(
        private componentFactoryResolver: ComponentFactoryResolver,
    ) {}

    @ViewChild('homeContainer', { static: false, read: ViewContainerRef }) componentContainer: ViewContainerRef;
    public cmpRef: any;
    public webStyle: string;
    ngAfterViewInit(): void {
        this.displayComponent();
    }

    ngOnInit(): void {
        this.webStyle = GlobalData.getInstance().getWebStyle;
    }

    private displayComponent(): void {
        if (this.componentContainer) {
            this.componentContainer.clear();
        }
        const styleKey: string = this.getStyleKey();

        const component = this.componentFactoryResolver.resolveComponentFactory(
            homeComponent[styleKey]
        );
        this.cmpRef = this.componentContainer.createComponent(component);
    }

    private getStyleKey(): string {
        for (const key in WebStyle) {
            if (Object.prototype.hasOwnProperty.call(WebStyle, key) && this.webStyle === WebStyle[key]) {
                return key;
            }
        }
        return 'Default';
    }

    ngOnDestroy() {
        if (this.cmpRef) {
            this.cmpRef.destroy();
        }
    }

}
