import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { IbmclogComponent } from './ibmc-log.component';

const routes: Routes = [
    {
        path: '',
        component: IbmclogComponent
    }
];

@NgModule({
    imports: [RouterModule.forChild(routes)],
    exports: [RouterModule]
})
export class IbmcLogRoutingModule { }
