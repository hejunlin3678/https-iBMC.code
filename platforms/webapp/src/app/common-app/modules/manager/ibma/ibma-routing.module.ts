import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { IbmaComponent } from './ibma.component';

const routes: Routes = [
    {
        path: '',
        component: IbmaComponent
    }
];

@NgModule({
    imports: [RouterModule.forChild(routes)],
    exports: [RouterModule]
})
export class IbmaRoutingModule { }
