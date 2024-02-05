import { NgModule } from '@angular/core';
import { BmcAppRoutingModule } from './bmc-app-routing.module';
import { BMCAppComponent } from './bmc-app.component';
import { NavMenuService } from './services/navMenu.service';
import { BMCGuard } from './guard/bmc-guard.service';
import { BMCAppService } from './bmc-app.service';

@NgModule({
    declarations: [BMCAppComponent],
    imports: [
        BmcAppRoutingModule
    ],
    providers: [
        BMCAppService,
        NavMenuService,
        BMCGuard
    ]
})
export class BmcAppModule { }
