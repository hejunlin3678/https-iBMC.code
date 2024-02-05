import { CommonModule } from '@angular/common';
import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { TpcmComponent } from './tpcm.component';

const routes: Routes = [
    {
        path: '',
        component: TpcmComponent
    }
];

@NgModule({
    imports: [RouterModule.forChild(routes)],
    exports: [RouterModule]
})
export class TpcmRoutingModule {}
