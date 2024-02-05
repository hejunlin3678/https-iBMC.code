import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { BiosComponent } from './bios.component';

const routes: Routes = [
    {
        path: '',
        component: BiosComponent
    }
];

@NgModule({
    imports: [RouterModule.forChild(routes)],
    exports: [RouterModule]
})
export class BiosRoutingModule { }
