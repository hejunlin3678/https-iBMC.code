import { NgModule } from '@angular/core';
import { EMMAppComponent } from './emm-app.component';
import { EMMGuard } from './guard/emm-guard.service';
import { EMMAppRoutingModule } from './emm-app-routing.module';
import { NavMenuService } from './services/navMenu.service';

@NgModule({
    declarations: [EMMAppComponent],
    imports: [
        EMMAppRoutingModule
    ],
    providers: [
        EMMGuard,
        NavMenuService
    ]
})
export class EMMAppModule { }
