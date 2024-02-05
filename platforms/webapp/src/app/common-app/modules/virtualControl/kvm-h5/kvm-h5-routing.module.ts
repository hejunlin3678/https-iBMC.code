import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { KvmH5Component } from './kvm-h5.component';

const routes: Routes = [
  {
      path: '',
      pathMatch: 'full',
      component: KvmH5Component
  }
];

@NgModule({
  imports: [RouterModule.forChild(routes)],
  exports: [RouterModule]
})
export class KvmH5RoutingModule { }
