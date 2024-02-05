import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { ConfigUpgradeComponent } from './config-upgrade.component';

const routes: Routes = [
    {
        path: '',
        component: ConfigUpgradeComponent
    }
];

@NgModule({
    imports: [RouterModule.forChild(routes)],
    exports: [RouterModule]
})
export class ConfigUpgradeRoutingModule { }
