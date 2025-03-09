import { HttpClientModule } from '@angular/common/http';
import { Component } from '@angular/core';
import { Router } from '@angular/router';
import { NzAvatarModule } from 'ng-zorro-antd/avatar';
import { NzButtonModule } from 'ng-zorro-antd/button';
import { NzIconModule } from 'ng-zorro-antd/icon';

@Component({
  selector: 'app-create-pet',
  imports: [
    NzButtonModule,
    HttpClientModule,
    NzIconModule,
    NzAvatarModule],
  templateUrl: './create-pet.component.html',
  styleUrl: './create-pet.component.scss'
})
export class CreatePetComponent {

  constructor(private router: Router) {}

  goToInstructions(): void {
    this.router.navigate(['/instrucciones']);
  }
}
