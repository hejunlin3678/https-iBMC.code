import { NgModule } from '@angular/core';
import { IrmAppRoutingModule } from './irm-app-routing.module';
import { IRMAppService } from './irm-app.service';
import { IRMAppComponent } from './irm-app.component';
import { NavMenuService } from './service/navMenu.service';
import { IRMGuard } from './guard/irm-guard.service';

@NgModule({
    declarations: [IRMAppComponent],
    imports: [
        IrmAppRoutingModule
    ],
    providers: [
        IRMAppService,
        NavMenuService,
        IRMGuard
    ]
})
export class IRMAppModule { }
