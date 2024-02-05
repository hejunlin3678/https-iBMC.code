import { NgModule } from '@angular/core';
import { HomeCollectComponent } from './home-collect.component';
import { TranslateModule } from '@ngx-translate/core';
@NgModule({
    declarations: [
        HomeCollectComponent,
    ],
    imports: [
        TranslateModule,
    ],
    exports: [
        HomeCollectComponent,
    ]
})
export class HomeCollectModule { }
