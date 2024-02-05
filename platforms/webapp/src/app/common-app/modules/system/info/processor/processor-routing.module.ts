import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { ProcessorComponent } from './processor.component';

const routes: Routes = [
    {
        path: '',
        component: ProcessorComponent
    }
];

@NgModule({
    imports: [RouterModule.forChild(routes)],
    exports: [RouterModule]
})
export class ProcessorRoutingModule { }
