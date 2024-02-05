import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { HomeLoginInforComponent } from './home-login-infor.component';
import { TranslateModule } from '@ngx-translate/core';
@NgModule({
    declarations: [
        HomeLoginInforComponent,
    ],
    imports: [
        CommonModule,
        TranslateModule,
    ],
    exports: [
        HomeLoginInforComponent,
    ]
})
export class HomeLoginInforModule { }
