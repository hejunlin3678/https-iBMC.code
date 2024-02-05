import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { PortComponent } from './port.component';

const routes: Routes = [
    {
        path: '',
        component: PortComponent
    }
];

@NgModule({
    imports: [RouterModule.forChild(routes)],
    exports: [RouterModule]
})
export class PortRoutingModule { }
