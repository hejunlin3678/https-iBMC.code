import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { WorknoteComponent } from './work-note.component';

const routes: Routes = [
    {
        path: '',
        component: WorknoteComponent
    }
];

@NgModule({
    imports: [RouterModule.forChild(routes)],
    exports: [RouterModule]
})
export class WorkNoteRoutingModule { }
