import { AfterViewInit, Component, ComponentFactoryResolver, OnDestroy, OnInit, ViewChild, ViewContainerRef } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { LoadingService } from 'src/app/common-app/service';
import { dynamicComponent } from './dynamicComponent';
import { NavTree } from './models/navTree';
import { UnitsService } from './units.service';

@Component({
  selector: 'app-units',
  templateUrl: './units.component.html',
  styleUrls: ['./units.component.scss']
})
export class UnitsComponent implements AfterViewInit, OnDestroy {

  @ViewChild('unitsContainer', { static: false, read: ViewContainerRef }) componentContainer: ViewContainerRef;

  public navTree: NavTree;
  public cmpRef: any;

  constructor(
      private unitsService: UnitsService,
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
          this.unitsService.activeCard = this.navTree.getCardUrlArray[index];
          this.navTree.getCardUrlArray.forEach((val) => val.setActive = false);
          this.navTree.getCardUrlArray[index].setActive = true;
          const component = this.componentFactoryResolver.resolveComponentFactory(
              dynamicComponent[this.navTree.getCardUrlArray[index].getComponentName]
          );
          this.cmpRef = this.componentContainer.createComponent(component);
      } catch (error) {
          this.loading.state.next(false);
      }
  }

  ngAfterViewInit() {
      this.loading.state.next(true);
      this.unitsService.init().subscribe(
          (val) => {
              this.navTree = this.unitsService.navTree;
                if (this.navTree.getCardUrlArray.length > 0) {
                    this.navTree.getCardUrlArray[0].setActive = true;
                    this.displayComponent(0);
                } else {
                    this.loading.state.next(false);
                }
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
