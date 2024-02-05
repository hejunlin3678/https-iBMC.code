import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { TranslateModule } from '@ngx-translate/core';
import { NetworkComponent } from './component/network.component';

const routes: Routes = [
    {
        path: '',
        component: NetworkComponent
    }
];

@NgModule({
    imports: [
        TranslateModule,
        RouterModule.forChild(routes)
    ],
    exports: [RouterModule]
})
export class BmcNetWorkRoutingModule { }
