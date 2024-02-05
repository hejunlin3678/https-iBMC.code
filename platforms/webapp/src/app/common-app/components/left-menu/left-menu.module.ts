import { CommonModule } from '@angular/common';
import { NgModule } from '@angular/core';
import { RouterModule } from '@angular/router';
import { LeftMenuComponent } from './left-menu.component';
import { TiLeftmenuModule, TiScrollModule } from '@cloud/tiny3';
import { TranslateModule } from '@ngx-translate/core';




@NgModule({
    declarations: [LeftMenuComponent],
    imports: [
        CommonModule,
        RouterModule,
        TiScrollModule,
        TiLeftmenuModule,
        TranslateModule,
    ],
    exports: [LeftMenuComponent]
})
export class LeftMenuModule { }
