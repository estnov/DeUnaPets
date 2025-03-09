import { Component } from '@angular/core';
import { Router } from '@angular/router';

@Component({
  selector: 'app-instructions',
  imports: [],
  templateUrl: './instructions.component.html',
  styleUrl: './instructions.component.scss'
})
export class InstructionsComponent {
  
  constructor(private router: Router) {}

  goHome(): void {
    this.router.navigate(['/home']);
  }
}
